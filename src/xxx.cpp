#include <stdio.h>
#include <iostream>
#include <math.h>
#include <ctime>

#include "wireshark/epan/epan.h"
#include <imgui.h>

#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pointer.h>

#include <Magnum/Timeline.h>
#include <Magnum/Math/Color.h>
#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Constants.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/Math/Vector3.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/MeshTools/Transform.h>
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

namespace Magnum { namespace Monopticon {

using namespace Math::Literals;

typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;

Vector2 randCirclePoint();

class ColoredDrawable: public SceneGraph::Drawable3D {
    public:
        explicit ColoredDrawable(Object3D& object, Shaders::Phong& shader, GL::Mesh& mesh, const Matrix4& primitiveTransformation, SceneGraph::DrawableGroup3D& drawables):
            SceneGraph::Drawable3D{object, &drawables},
            _shader(shader),
            _mesh(mesh),
            _primitiveTransformation{primitiveTransformation} {}

    private:
        void draw(const Matrix4& transformation, SceneGraph::Camera3D& camera) override {
            _shader.setTransformationMatrix(transformation*_primitiveTransformation)
                   .setNormalMatrix(transformation.rotationScaling())
                   .setProjectionMatrix(camera.projectionMatrix());
            _mesh.draw(_shader);
        }

        Shaders::Phong& _shader;
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

    private:
        ImGuiIntegration::Context _imgui{NoCreate};

        GL::Mesh _box{NoCreate}, _circle{NoCreate};
        Color4 _clearColor = 0x002b36_rgbf;
        Color4 _pickColor = 0xffffff_rgbf;

        GL::Buffer _indexBuffer, _vertexBuffer;
        GL::Mesh _mesh;
        Shaders::Phong _shader;

        Shaders::Flat3D _line_shader;

        Scene3D _scene;
        SceneGraph::Camera3D* _camera;
        SceneGraph::DrawableGroup3D _drawables;
        Timeline _timeline;

        Object3D *_cameraRig, *_cameraObject;
        Vector2i _previousMousePosition;
        Color3 _color;

        bool _drawCubes{true};
};


ObjSelect::ObjSelect(const Arguments& arguments): Platform::Application(arguments,
            Configuration{}.setTitle("Monopticon").setWindowFlags(Configuration::WindowFlag::Borderless).setSize(Vector2i{1200,800}),
            GLConfiguration{}.setSampleCount(16)) {
    {
    std::cout << "Made it here" << std::endl;

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
    _shader = Shaders::Phong{};
    _shader.setAmbientColor(0x111111_rgbf)
           .setSpecularColor(0x330000_rgbf)
           .setDiffuseColor(_pickColor)
           .setLightPosition({10.0f, 15.0f, 5.0f});

    _line_shader = Shaders::Flat3D{};
    _line_shader.setColor(0x00ffff_rgbf);

    srand(time(NULL));
    for(Int i = 0; i != 10; ++i) {
        Object3D* o = new Object3D{&_scene};
        Vector2 v = 10.0f*randCirclePoint();
        o->translate({v.x(), 0.0f, v.y()});

        new ColoredDrawable{*o, _shader, _box,
            Matrix4::scaling(Vector3{0.25f}), _drawables};

        Object3D* line = new Object3D{&_scene};
        Vector3 a = Vector3{0.0f, 0.0f, 0.0f};
        Vector3 b = Vector3{v.x(), 0.0f, v.y()};

        new PacketLineDrawable{*line, _line_shader, a, b, _drawables};
    }

    Object3D *cow = new Object3D{&_scene};
    Matrix4 scaling = Matrix4::scaling(Vector3{10});
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

    setSwapInterval(1);
    setMinimalLoopPeriod(16);
    _timeline.start();
    }
}

void ObjSelect::drawEvent() {
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color|GL::FramebufferClear::Depth);

    GL::Renderer::setClearColor(_clearColor);

    /* Draw all the things */
    if(_drawCubes) _camera->draw(_drawables);
    _cameraObject->rotateY(0.10_degf);

    _imgui.newFrame();

    ImGui::SetNextWindowSize(ImVec2(300, 40), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Heads Up Display");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
        1000.0/Double(ImGui::GetIO().Framerate), Double(ImGui::GetIO().Framerate));
    if(ImGui::ColorEdit3("Pick Item Color", _pickColor.data()))
        _shader.setDiffuseColor(_pickColor);
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

    _previousMousePosition = event.position();
    event.setAccepted();
}

void ObjSelect::mouseReleaseEvent(MouseEvent& event) {
    if(_imgui.handleMouseReleaseEvent(event)) return;
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
