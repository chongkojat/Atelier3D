#pragma once

#include "Engine/Platform/Window.hpp"

#include <memory>

namespace Engine::Core
{
    struct ApplicationSpec
    {
        Platform::WindowSpec window;
        int maxFrames = 0;
    };

    class Application
    {
    public:
        explicit Application(ApplicationSpec spec = {});
        virtual ~Application();

        void Run();
        void Close();

        Platform::Window& GetWindow();
        const Platform::Window& GetWindow() const;

    protected:
        virtual void OnInit() {}
        virtual void OnShutdown() {}
        virtual void OnUpdate(double deltaSeconds) {}

    private:
        ApplicationSpec spec_;
        std::unique_ptr<Platform::Window> window_;
        bool running_ = true;
        int frameCount_ = 0;
    };
} 
