#include "Engine/Platform/GlfwWindow.hpp"

#include "Engine/Core/Log.hpp"

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <string>

namespace Engine::Platform
{
    namespace
    {
        void ErrorCallback(int code, const char* description)
        {
            Core::Log(Core::LogLevel::Error, "GLFW error " + std::to_string(code) + ": " + description);
        }
    }

    GlfwWindow::GlfwWindow(WindowSpec spec) : spec_(std::move(spec))
    {
        glfwSetErrorCallback(&ErrorCallback);
        if (!glfwInit())
        {
            throw std::runtime_error("glfwInit failed");
        }

        glfwWindowHint(GLFW_DEPTH_BITS, 24);
        glfwWindowHint(GLFW_STENCIL_BITS, 8);

        window_ = glfwCreateWindow(spec_.width, spec_.height, spec_.title.c_str(), nullptr, nullptr);
        if (!window_)
        {
            glfwTerminate();
            throw std::runtime_error("glfwCreateWindow failed");
        }

        glfwSetWindowUserPointer(window_, this);
        glfwSetFramebufferSizeCallback(window_, &GlfwWindow::FramebufferSizeCallback);
        glfwSetKeyCallback(window_, &GlfwWindow::KeyCallback);

        glfwMakeContextCurrent(window_);
        glfwSwapInterval(1);

        glfwGetFramebufferSize(window_, &spec_.width, &spec_.height);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }

    GlfwWindow::~GlfwWindow()
    {
        if (window_)
        {
            glfwDestroyWindow(window_);
            window_ = nullptr;
        }
        glfwTerminate();
    }

    void GlfwWindow::PollEvents()
    {
        glfwPollEvents();
        if (glfwWindowShouldClose(window_))
        {
            shouldClose_ = true;
        }
    }

    void GlfwWindow::Present()
    {
        glfwSwapBuffers(window_);
    }

    bool GlfwWindow::ShouldClose() const
    {
        return shouldClose_;
    }

    void GlfwWindow::RequestClose()
    {
        shouldClose_ = true;
        if (window_)
        {
            glfwSetWindowShouldClose(window_, GLFW_TRUE);
        }
    }

    int GlfwWindow::Width() const
    {
        return spec_.width;
    }

    int GlfwWindow::Height() const
    {
        return spec_.height;
    }

    const std::string& GlfwWindow::Title() const
    {
        return spec_.title;
    }

    void* GlfwWindow::NativeHandle() const
    {
        return window_;
    }

    void GlfwWindow::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
    {
        auto* self = static_cast<GlfwWindow*>(glfwGetWindowUserPointer(window));
        if (!self)
        {
            return;
        }

        self->spec_.width = width > 0 ? width : 1;
        self->spec_.height = height > 0 ? height : 1;
    }

    void GlfwWindow::KeyCallback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
    {
        auto* self = static_cast<GlfwWindow*>(glfwGetWindowUserPointer(window));
        if (self && key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            self->RequestClose();
        }
    }
}
