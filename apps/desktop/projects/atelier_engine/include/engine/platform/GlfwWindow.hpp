#pragma once

#include "engine/platform/Window.hpp"

struct GLFWwindow;

namespace Engine::Platform
{
    // Window + OpenGL context for non-Windows platforms (Linux, macOS)
    class GlfwWindow final : public Window
    {
    public:
        explicit GlfwWindow(WindowSpec spec);
        ~GlfwWindow() override;

        void PollEvents() override;
        void Present() override;
        bool ShouldClose() const override;
        void RequestClose() override;

        int Width() const override;
        int Height() const override;
        const std::string& Title() const override;

        void* NativeHandle() const override;

    private:
        static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
        static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

        WindowSpec spec_;
        bool shouldClose_ = false;

        GLFWwindow* window_ = nullptr;
    };
}
