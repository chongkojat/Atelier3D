#pragma once

#include "Engine/Game/GameProject.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include <memory>

namespace Engine::Platform
{
    class Window;
}

namespace Engine::Game
{
    class GameRuntime
    {
    public:
        GameRuntime();

        void LoadProject(GameProject project);
        void Start();
        void Stop();
        bool IsRunning() const;

        void Tick(double deltaSeconds, const Platform::Window& window);

    private:
        std::unique_ptr<GameProject> project_;
        SceneDescription scene_;
        Renderer::Renderer renderer_;
        bool running_ = false;
        float cubeAngleDegrees_ = 0.0f;
    };
}

