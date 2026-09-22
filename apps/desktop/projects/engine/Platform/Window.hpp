#pragma once

#include <memory>
#include <string>

namespace Engine::Platform
{
    struct WindowSpec
    {
        int width = 1280;
        int height = 720;
        std::string title = "GAM300";
    };

    class Window
    {
    public:
        virtual ~Window() = default;

        virtual void PollEvents() = 0;
        virtual void Present() = 0;
        virtual bool ShouldClose() const = 0;
        virtual void RequestClose() = 0;

        virtual int Width() const = 0;
        virtual int Height() const = 0;
        virtual const std::string& Title() const = 0;
    };

    std::unique_ptr<Window> CreateWindow(const WindowSpec& spec);
}
