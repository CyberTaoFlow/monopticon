#include <stdio.h>
#include <iostream>
#include <math.h>
#include <ctime>

#include <imgui.h>

#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pointer.h>
#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>

#include <Magnum/Timeline.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Image.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Framebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/GL/RenderbufferFormat.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Constants.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/Math/Vector3.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/MeshTools/Transform.h>
#include <Magnum/MeshTools/CompressIndices.h>
#include <Magnum/MeshTools/Interleave.h>
#include <Magnum/ImGuiIntegration/Context.hpp>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/Primitives/Circle.h>
#include <Magnum/Primitives/Line.h>
#include <Magnum/Primitives/UVSphere.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Shaders/MeshVisualizer.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Trade/MeshData3D.h>

#include "broker/broker.hh"
#include "broker/message.hh"
#include "broker/bro.hh"

#include <unordered_map>

// Zeek broker components
broker::endpoint ep;
broker::subscriber subscriber = ep.make_subscriber({"monopt/l2"});

namespace Magnum { namespace Monopticon {

void parse_raw_packet(broker::bro::Event event);

using namespace Math::Literals;

typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;

Vector2 randCirclePoint();

struct DeviceStats {
  std::string mac_addr;
  Vector2 CircPoint;
  int num_pkts_sent;
  int num_pkts_recv;
};

std::unordered_map<std::string, DeviceStats> device_map = {};


class PhongIdShader: public GL::AbstractShaderProgram {
    public:
        typedef GL::Attribute<0, Vector3> Position;
        typedef GL::Attribute<1, Vector3> Normal;

        enum: UnsignedInt {
            ColorOutput = 0,
            ObjectIdOutput = 1
        };

        explicit PhongIdShader();

        PhongIdShader& setObjectId(UnsignedInt id) {
            setUniform(_objectIdUniform, id);
            return *this;
        }

        PhongIdShader& setLightPosition(const Vector3& position) {
            setUniform(_lightPositionUniform, position);
            return *this;
        }

        PhongIdShader& setAmbientColor(const Color3& color) {
            setUniform(_ambientColorUniform, color);
            return *this;
        }

        PhongIdShader& setColor(const Color3& color) {
            setUniform(_colorUniform, color);
            return *this;
        }

        PhongIdShader& setTransformationMatrix(const Matrix4& matrix) {
            setUniform(_transformationMatrixUniform, matrix);
            return *this;
        }

        PhongIdShader& setNormalMatrix(const Matrix3x3& matrix) {
            setUniform(_normalMatrixUniform, matrix);
            return *this;
        }

        PhongIdShader& setProjectionMatrix(const Matrix4& matrix) {
            setUniform(_projectionMatrixUniform, matrix);
            return *this;
        }

    private:
        Int _objectIdUniform,
            _lightPositionUniform,
            _ambientColorUniform,
            _colorUniform,
            _transformationMatrixUniform,
            _normalMatrixUniform,
            _projectionMatrixUniform;
};

PhongIdShader::PhongIdShader() {
    Utility::Resource rs("picking-data");

    GL::Shader vert{GL::Version::GL330, GL::Shader::Type::Vertex},
        frag{GL::Version::GL330, GL::Shader::Type::Fragment};
    vert.addSource(rs.get("PhongId.vert"));
    frag.addSource(rs.get("PhongId.frag"));
    CORRADE_INTERNAL_ASSERT(GL::Shader::compile({vert, frag}));
    attachShaders({vert, frag});
    CORRADE_INTERNAL_ASSERT(link());

    _objectIdUniform = uniformLocation("objectId");
    _lightPositionUniform = uniformLocation("light");
    _ambientColorUniform = uniformLocation("ambientColor");
    _colorUniform = uniformLocation("color");
    _transformationMatrixUniform = uniformLocation("transformationMatrix");
    _projectionMatrixUniform = uniformLocation("projectionMatrix");
    _normalMatrixUniform = uniformLocation("normalMatrix");
}


class DeviceDrawable: public SceneGraph::Drawable3D {
    public:
        explicit DeviceDrawable(UnsignedByte id, Object3D& object, PhongIdShader& shader, Color3& color, GL::Mesh& mesh, const Matrix4& primitiveTransformation, SceneGraph::DrawableGroup3D& drawables):
            SceneGraph::Drawable3D{object, &drawables},
            _id{id},
            _selected{false},
            _color{color},
            _shader(shader),
            _mesh(mesh),
            _primitiveTransformation{primitiveTransformation} {}

        void setSelected(bool selected) { _selected = selected; }

    private:
        void draw(const Matrix4& transformation, SceneGraph::Camera3D& camera) override {
            _shader.setTransformationMatrix(transformation*_primitiveTransformation)
                   .setNormalMatrix(transformation.rotationScaling())
                   .setProjectionMatrix(camera.projectionMatrix())
                   .setAmbientColor(_selected ? _color*0.9f : Color3{})
                   .setColor(_color*(_selected ? 2.0f : 1.7f))
                   /* relative to the camera */
                   .setLightPosition({0.0f, 5.0f, 2.0f})
                   .setObjectId(_id);
            _mesh.draw(_shader);
        }


        UnsignedByte _id;
        bool _selected;
        Color3 _color;
        PhongIdShader& _shader;
        GL::Mesh& _mesh;
        Matrix4 _primitiveTransformation;
};


class RingDrawable: public SceneGraph::Drawable3D {
    public:
        explicit RingDrawable(Object3D& object, const Color4& color, SceneGraph::DrawableGroup3D& group):
            SceneGraph::Drawable3D{object, &group}
        {

            _mesh = MeshTools::compile(Primitives::circle3DWireframe(70));
            _color = color;
            _shader = Shaders::MeshVisualizer{Shaders::MeshVisualizer::Flag::Wireframe|Shaders::MeshVisualizer::Flag::NoGeometryShader};
        }

    private:
        void draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) override {
            using namespace Math::Literals;

            _shader.setColor(0xffffff_rgbf)
                   .setWireframeColor(_color)
                   .setTransformationProjectionMatrix(camera.projectionMatrix()*transformationMatrix);
            _mesh.draw(_shader);
        }

        GL::Mesh _mesh;
        Color4 _color;
        Shaders::MeshVisualizer _shader;
};


class PacketLineDrawable: public SceneGraph::Drawable3D {
    public:
        explicit PacketLineDrawable(Object3D& object, Shaders::Flat3D& shader, Vector3& a, Vector3& b, SceneGraph::DrawableGroup3D& group):
            SceneGraph::Drawable3D{object, &group},
            _shader{shader}
        {

            _mesh = MeshTools::compile(Primitives::line3D(a,b));
        }

    private:
        void draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) override {
            _shader.setTransformationProjectionMatrix(camera.projectionMatrix()*transformationMatrix);
            _mesh.draw(_shader);
        }

        GL::Mesh _mesh;
        Shaders::Flat3D& _shader;
};


class ObjSelect: public Platform::Application {
    public:
        explicit ObjSelect(const Arguments& arguments);

        void drawEvent() override;

        void viewportEvent(ViewportEvent& event) override;

        void keyPressEvent(KeyEvent& event) override;
        void keyReleaseEvent(KeyEvent& event) override;

        void mousePressEvent(MouseEvent& event) override;
        void mouseReleaseEvent(MouseEvent& event) override;
        void mouseMoveEvent(MouseMoveEvent& event) override;
        void mouseScrollEvent(MouseScrollEvent& event) override;
        void textInputEvent(TextInputEvent& event) override;

        void parse_raw_packet(broker::bro::Event event);
        Vector2 createCircle();
        void createLine(Vector2, Vector2);

    private:
        // UI fields
        ImGuiIntegration::Context _imgui{NoCreate};

        // Graphic fields
        GL::Mesh _box{NoCreate}, _circle{NoCreate};
        Color4 _clearColor = 0x002b36_rgbf;
        Color4 _pickColor = 0xffffff_rgbf;

        GL::Buffer _indexBuffer, _vertexBuffer;
        GL::Mesh _mesh;
        PhongIdShader _shader;

        Shaders::Flat3D _line_shader;

        Scene3D _scene;
        SceneGraph::Camera3D* _camera;
        SceneGraph::DrawableGroup3D _drawables;
        Timeline _timeline;

        Object3D *_cameraRig, *_cameraObject;

        GL::Framebuffer _framebuffer;
        Vector2i _previousMousePosition, _mousePressPosition;
        Color3 _color;
        GL::Renderbuffer _colorBuf, _objectId, _depth;


        std::vector<DeviceDrawable*> _device_objects{};

        bool _drawCubes{true};
};


ObjSelect::ObjSelect(const Arguments& arguments):
        Platform::Application{arguments, Configuration{}
            .setTitle("Monopticon")
            .setWindowFlags(Configuration::WindowFlag::Borderless|Configuration::WindowFlag::Resizable)
            .setSize(Vector2i{1200,800}),
            GLConfiguration{}.setSampleCount(16)},
        _framebuffer{GL::defaultFramebuffer.viewport()}
{
    std::cout << "Waiting for broker connection" << std::endl;

    ep.listen("127.0.0.1", 9999);

    std::cout << "Connection received" << std::endl;

    /* Global renderer configuration */
    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);

    /* Configure framebuffer (using R8UI for object ID which means 255 objects max) */
    _colorBuf.setStorage(GL::RenderbufferFormat::RGBA8, GL::defaultFramebuffer.viewport().size());
    _objectId.setStorage(GL::RenderbufferFormat::R8UI, GL::defaultFramebuffer.viewport().size());
    _depth.setStorage(GL::RenderbufferFormat::DepthComponent24, GL::defaultFramebuffer.viewport().size());
    _framebuffer.attachRenderbuffer(GL::Framebuffer::ColorAttachment{0}, _colorBuf)
                .attachRenderbuffer(GL::Framebuffer::ColorAttachment{1}, _objectId)
                .attachRenderbuffer(GL::Framebuffer::BufferAttachment::Depth, _depth)
                .mapForDraw({{PhongIdShader::ColorOutput, GL::Framebuffer::ColorAttachment{0}},
                            {PhongIdShader::ObjectIdOutput, GL::Framebuffer::ColorAttachment{1}}});
    CORRADE_INTERNAL_ASSERT(_framebuffer.checkStatus(GL::FramebufferTarget::Draw) == GL::Framebuffer::Status::Complete);

    /* Camera setup */
    (*(_cameraRig = new Object3D{&_scene}))
        .translate(Vector3::yAxis(3.0f))
        .rotateY(40.0_degf);
    (*(_cameraObject = new Object3D{_cameraRig}))
        .translate(Vector3::zAxis(30.0f))
        .rotateX(-25.0_degf);
    (_camera = new SceneGraph::Camera3D(*_cameraObject))
        ->setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
        .setProjectionMatrix(Matrix4::perspectiveProjection(25.0_degf, 1.0f, 0.001f, 100.0f))
        .setViewport(GL::defaultFramebuffer.viewport().size()); /* Drawing setup */

    _box = MeshTools::compile(Primitives::uvSphereSolid(8.0f, 30.0f));

    _line_shader = Shaders::Flat3D{};
    _line_shader.setColor(0x00ffff_rgbf);
    _shader = PhongIdShader{};

    srand(time(NULL));

    Object3D *cow = new Object3D{&_scene};
    Matrix4 scaling = Matrix4::scaling(Vector3{100});
    cow->transform(scaling);
    cow->rotateX(60.0_degf);
    new RingDrawable{*cow, 0x0000ff_rgbf,_drawables};

    Object3D *cor = new Object3D{&_scene};
    cor->rotateX(90.0_degf);
    cor->transform(scaling);
    new RingDrawable{*cor, 0xff000088_rgbaf,_drawables};

    Object3D *cog = new Object3D{&_scene};
    cog->rotateX(-60.0_degf);
    cog->transform(scaling);
    new RingDrawable{*cog, 0x00ff0088_rgbaf,_drawables};

    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::enable(GL::Renderer::Feature::PolygonOffsetFill);
    GL::Renderer::setPolygonOffset(2.0f, 0.5f);

    _imgui = ImGuiIntegration::Context(Vector2{windowSize()}/dpiScaling(),
        windowSize(), framebufferSize());

    GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add,
        GL::Renderer::BlendEquation::Add);
    GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
        GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);

    Vector2 p1 = createCircle();
    std::string mac_dst = "ba:dd:be:ee:ef";
    DeviceStats *d_s = new DeviceStats{mac_dst, p1, 0, 1};
    device_map.insert(std::make_pair(mac_dst, *d_s));

    Vector2 p2 = createCircle();
    mac_dst = "ca:ff:eb:ee:ef";
    d_s = new DeviceStats{mac_dst, p2, 0, 1};
    device_map.insert(std::make_pair(mac_dst, *d_s));


    setSwapInterval(1);
    setMinimalLoopPeriod(16);
    _timeline.start();
}


void ObjSelect::parse_raw_packet(broker::bro::Event event) {
    broker::vector parent_content = event.args();

    broker::vector *raw_pkt_hdr = broker::get_if<broker::vector>(parent_content.at(0));
    if (raw_pkt_hdr == NULL) {
        std::cout << "rph" << std::endl;
        return;
    }
    broker::vector *l2_pkt_hdr = broker::get_if<broker::vector>(raw_pkt_hdr->at(0));
    if (l2_pkt_hdr == NULL || l2_pkt_hdr->size() != 9) {
        std::cout << "lph" << std::endl;
        return;
    }

    std::string *mac_src = broker::get_if<std::string>(l2_pkt_hdr->at(3));
    if (mac_src == NULL) {
        std::cout << "mac_src" << std::endl;
        return;
    }
    std::string *mac_dst = broker::get_if<std::string>(l2_pkt_hdr->at(4));

    Vector2 p1;
    DeviceStats *d_s;

    auto search = device_map.find(*mac_src);
    if (search == device_map.end()) {
        p1 = createCircle();
        d_s = new DeviceStats{*mac_src, p1, 1, 0};
        device_map.insert(std::make_pair(*mac_src, *d_s));
    } else {
        d_s = &(search->second);
        p1 = d_s->CircPoint;
        d_s->num_pkts_sent += 1;
    }

    Vector2 p2;
    auto search_dst = device_map.find(*mac_dst);
    if (search_dst == device_map.end()) {
            p2 = createCircle();
            d_s = new DeviceStats{*mac_dst, p2, 0, 1};
            device_map.insert(std::make_pair(*mac_dst, *d_s));
    } else {
        p2 = search_dst->second.CircPoint;
    }
    createLine(p1, p2);
}


Vector2 ObjSelect::createCircle() {
    Object3D* o = new Object3D{&_scene};


    Vector2 v = 10.0f*randCirclePoint();
    o->translate({v.x(), 0.0f, v.y()});

    Color3 c = 0x2f83cc_rgbf;
    UnsignedByte id = static_cast<UnsignedByte>(_device_objects.size());

    DeviceDrawable *d = new DeviceDrawable{id, *o, _shader, c, _box,
        Matrix4::scaling(Vector3{0.25f}), _drawables};

    _device_objects.push_back(d);

    return v;
}


void ObjSelect::createLine(Vector2 a, Vector2 b) {
        Object3D* line = new Object3D{&_scene};
        Vector3 a3 = Vector3{a.x(), 0.0f, a.y()};
        Vector3 b3 = Vector3{b.x(), 0.0f, b.y()};

        new PacketLineDrawable{*line, _line_shader, a3, b3, _drawables};
}


void ObjSelect::drawEvent() {
    for (auto msg : subscriber.poll()) {
        broker::topic topic = broker::get_topic(msg);
        broker::bro::Event event = broker::get_data(msg);
        std::cout << "received on topic: " << topic << " event: " << event.args() << std::endl;
        if (event.name().compare("raw_packet_event")) {
            parse_raw_packet(event);
        } else {
            std::cout << "Unhandled Event: " << event.name() << std::endl;
        }
    }

    // Actually draw things

    /* Draw to custom framebuffer */
    _framebuffer
        .clearColor(0, Color3{0.125f})
        .clearColor(1, Vector4ui{})
        .clearDepth(1.0f)
        .bind();
    _camera->draw(_drawables);


    /* Draw all the things */
    if(_drawCubes) _camera->draw(_drawables);




    /* Bind the main buffer back */
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color|GL::FramebufferClear::Depth)
        .bind();

    GL::Renderer::setClearColor(_clearColor);

    /* Blit color to window framebuffer */
    _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{0});
    GL::AbstractFramebuffer::blit(_framebuffer, GL::defaultFramebuffer,
        {{}, _framebuffer.viewport().size()}, GL::FramebufferBlit::Color);

    /* Rotate the camera on an orbit */
    //_cameraObject->rotateY(0.10_degf);

    _imgui.newFrame();

    ImGui::SetNextWindowSize(ImVec2(300, 40), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Heads Up Display");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
        1000.0/Double(ImGui::GetIO().Framerate), Double(ImGui::GetIO().Framerate));
    //if(ImGui::ColorEdit3("Pick Item Color", _pickColor.data()))
        //_shader.setDiffuseColor(&_pickColor);
    if(ImGui::ColorEdit3("Pick Background Color", _clearColor.data()))
        GL::Renderer::setClearColor(_clearColor);
    ImGui::End();

    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::disable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::ScissorTest);

    _imgui.drawFrame();

    GL::Renderer::disable(GL::Renderer::Feature::ScissorTest);
    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::disable(GL::Renderer::Feature::Blending);

    swapBuffers();
    _timeline.nextFrame();
    redraw();
}


void ObjSelect::viewportEvent(ViewportEvent& event) {
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});

    _imgui.relayout(Vector2{event.windowSize()}/event.dpiScaling(),
        event.windowSize(), event.framebufferSize());
}


void ObjSelect::keyPressEvent(KeyEvent& event) {
    if(_imgui.handleKeyPressEvent(event)) return;



    /* Movement */
    if(event.key() == KeyEvent::Key::Down) {
        _cameraObject->rotateX(5.0_degf);
    } else if(event.key() == KeyEvent::Key::Up) {
        _cameraObject->rotateX(-5.0_degf);
    } else if(event.key() == KeyEvent::Key::Left) {
        _cameraRig->rotateY(-5.0_degf);
    } else if(event.key() == KeyEvent::Key::Right) {
        _cameraRig->rotateY(5.0_degf);
    }
}


void ObjSelect::keyReleaseEvent(KeyEvent& event) {
    if(_imgui.handleKeyReleaseEvent(event)) return;
}


void ObjSelect::mousePressEvent(MouseEvent& event) {
    if(_imgui.handleMousePressEvent(event)) return;

    if(event.button() != MouseEvent::Button::Left) return;

    _previousMousePosition = _mousePressPosition = event.position();

    event.setAccepted();
}

void ObjSelect::mouseReleaseEvent(MouseEvent& event) {
    if(_imgui.handleMouseReleaseEvent(event)) return;

    if(event.button() != MouseEvent::Button::Left || _mousePressPosition != event.position()) return;

    /* Read object ID at given click position (framebuffer has Y up while windowing system Y down) */
    _framebuffer.mapForRead(GL::Framebuffer::ColorAttachment{1});
    Image2D data = _framebuffer.read(
        Range2Di::fromSize({event.position().x(), _framebuffer.viewport().sizeY() - event.position().y() - 1}, {1, 1}),
        {PixelFormat::R8UI});

    /* Highlight object under mouse and deselect all other ...
     * TODO check functionality */
    for(std::vector<DeviceDrawable*>::iterator it = _device_objects.begin(); it != _device_objects.end(); ++it) {
        (*it)->setSelected(false);
    }

    UnsignedByte id = data.data<UnsignedByte>()[0];
    std::cout << "Printing id: " << id << std::endl;
    printf("%d\n", id);
    if(id > -1 && id < _device_objects.size()) {
        _device_objects.at(id)->setSelected(true);
    }

    event.setAccepted();
    redraw();
}


void ObjSelect::mouseMoveEvent(MouseMoveEvent& event) {
    if(_imgui.handleMouseMoveEvent(event)) return;

    if(!(event.buttons() & MouseMoveEvent::Button::Left)) return;

    const Vector2 delta = 3.0f*
        Vector2{event.position() - _previousMousePosition}/
        Vector2{GL::defaultFramebuffer.viewport().size()};

    (*_cameraObject)
        .rotate(Rad{-delta.y()}, _cameraObject->transformation().right().normalized())
        .rotateY(Rad{-delta.x()});

    _previousMousePosition = event.position();
    event.setAccepted();
    redraw();
}


void ObjSelect::mouseScrollEvent(MouseScrollEvent& event) {
    if(_imgui.handleMouseScrollEvent(event)) return;

    if(!event.offset().y()) return;

    /* Distance to origin */
    const Float distance = _cameraObject->transformation().translation().z();

    /* Move 15% of the distance back or forward */
    _cameraObject->translate(Vector3::zAxis(
        distance*(1.0f - (event.offset().y() > 0 ? 1/0.85f : 0.85f))));

    redraw();
}


void ObjSelect::textInputEvent(TextInputEvent& event) {
    if(_imgui.handleTextInputEvent(event)) return;
}


Vector2 randCirclePoint() {
    float f =  rand() / (RAND_MAX/(2*Math::Constants<float>::pi()));

    return Vector2{cos(f), sin(f)};
}

}}

MAGNUM_APPLICATION_MAIN(Magnum::Monopticon::ObjSelect)
