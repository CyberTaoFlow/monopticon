#include <stdio.h>

#include "wireshark/epan/epan.h"

#include <imgui.h>
#include <Magnum/Math/Color.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/ImGuiIntegration/Context.hpp>

#include <Magnum/Platform/Sdl2Application.h>

namespace Magnum { namespace Examples {

using namespace Math::Literals;

class ImGuiExample: public Platform::Application {
    public:
        explicit ImGuiExample(const Arguments& arguments);

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

        bool _showTestWindow = true;
        bool _showAnotherWindow = false;
        Color4 _clearColor = 0x72909aff_rgbaf;
        Float _floatValue = 0.0f;
};

ImGuiExample::ImGuiExample(const Arguments& arguments): Platform::Application{arguments,
    Configuration{}.setTitle("Magnum ImGui Example")
                   .setWindowFlags(Configuration::WindowFlag::Resizable)}
{
    _imgui = ImGuiIntegration::Context(Vector2{windowSize()}/dpiScaling(),
        windowSize(), framebufferSize());

    /* Set up proper blending to be used by ImGui. There's a great chance
       you'll need this exact behavior for the rest of your scene. If not, set
       this only for the drawFrame() call. */
    GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add,
        GL::Renderer::BlendEquation::Add);
    GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
        GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    #if !defined(MAGNUM_TARGET_WEBGL) && !defined(CORRADE_TARGET_ANDROID)
    /* Have some sane speed, please */
    setMinimalLoopPeriod(16);
    #endif
}

void ImGuiExample::drawEvent() {
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

    _imgui.newFrame();

    /* Enable text input, if needed */
    if(ImGui::GetIO().WantTextInput && !isTextInputActive())
        startTextInput();
    else if(!ImGui::GetIO().WantTextInput && isTextInputActive())
        stopTextInput();

    /* 1. Show a simple window.
       Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appear in
       a window called "Debug" automatically */
    {
        ImGui::Text("Hello, world!");
        ImGui::SliderFloat("Float", &_floatValue, 0.0f, 1.0f);
        if(ImGui::ColorEdit3("Clear Color", _clearColor.data()))
            GL::Renderer::setClearColor(_clearColor);
        if(ImGui::Button("Test Window"))
            _showTestWindow ^= true;
        if(ImGui::Button("Another Window"))
            _showAnotherWindow ^= true;
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
            1000.0/Double(ImGui::GetIO().Framerate), Double(ImGui::GetIO().Framerate));
    }

    /* 2. Show another simple window, now using an explicit Begin/End pair */
    if(_showAnotherWindow) {
        ImGui::SetNextWindowSize(ImVec2(500, 100), ImGuiSetCond_FirstUseEver);
        ImGui::Begin("Another Window", &_showAnotherWindow);
        ImGui::Text("Hello");
        ImGui::End();
    }

    /* 3. Show the ImGui test window. Most of the sample code is in
       ImGui::ShowTestWindow() */
    if(_showTestWindow) {
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
        ImGui::ShowTestWindow();
    }

    /* Set appropriate states. If you only draw imgui UI, it is sufficient to
       do this once in the constructor. */
    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::disable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::ScissorTest);

    _imgui.drawFrame();

    /* Reset state. Only needed if you want to draw something else with
       different state next frame. */
    GL::Renderer::disable(GL::Renderer::Feature::ScissorTest);
    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::disable(GL::Renderer::Feature::Blending);

    swapBuffers();
    redraw();
}

void ImGuiExample::viewportEvent(ViewportEvent& event) {
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});

    _imgui.relayout(Vector2{event.windowSize()}/event.dpiScaling(),
        event.windowSize(), event.framebufferSize());
}

void ImGuiExample::keyPressEvent(KeyEvent& event) {
    if(_imgui.handleKeyPressEvent(event)) return;
}

void ImGuiExample::keyReleaseEvent(KeyEvent& event) {
    if(_imgui.handleKeyReleaseEvent(event)) return;
}

void ImGuiExample::mousePressEvent(MouseEvent& event) {
    if(_imgui.handleMousePressEvent(event)) return;
}

void ImGuiExample::mouseReleaseEvent(MouseEvent& event) {
    if(_imgui.handleMouseReleaseEvent(event)) return;
}

void ImGuiExample::mouseMoveEvent(MouseMoveEvent& event) {
    if(_imgui.handleMouseMoveEvent(event)) return;
}

void ImGuiExample::mouseScrollEvent(MouseScrollEvent& event) {
    if(_imgui.handleMouseScrollEvent(event)) return;
}

void ImGuiExample::textInputEvent(TextInputEvent& event) {
    if(_imgui.handleTextInputEvent(event)) return;
}

}}

MAGNUM_APPLICATION_MAIN(Magnum::Examples::ImGuiExample)
