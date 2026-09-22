#include "Engine/Game/GameRuntime.hpp"

#include "Engine/Platform/Window.hpp"

namespace Engine::Game
{
    GameRuntime::GameRuntime()
    {
        renderer_.Init();
        renderer_.SetClearColor(0.08f, 0.09f, 0.11f, 1.0f);
    }

    void GameRuntime::LoadProject(GameProject project)
    {
        project_ = std::make_unique<GameProject>(std::move(project));
    }

    void GameRuntime::Start()
    {
        if (!project_)
        {
            LoadProject(GameProject(GameProject::FindDefaultGameProjectDir()));
        }

        scene_ = project_->LoadDefaultScene();
        cubeAngleDegrees_ = 0.0f;
        running_ = true;
    }

    void GameRuntime::Stop()
    {
        running_ = false;
    }

    bool GameRuntime::IsRunning() const
    {
        return running_;
    }

    void GameRuntime::Tick(double deltaSeconds, const Platform::Window& window)
    {
        renderer_.BeginFrame(window.Width(), window.Height());
        renderer_.Clear();

        if (!running_)
        {
            return;
        }

        if (scene_.hasCube)
        {
            cubeAngleDegrees_ += static_cast<float>(deltaSeconds) * scene_.cubeRotationSpeedDegreesPerSecond;
            if (cubeAngleDegrees_ >= 360.0f)
            {
                cubeAngleDegrees_ -= 360.0f;
            }
            renderer_.DrawCube(cubeAngleDegrees_);
        }
        else if (scene_.hasPyramid)
        {
            cubeAngleDegrees_ += static_cast<float>(deltaSeconds) * scene_.cubeRotationSpeedDegreesPerSecond;
            if (cubeAngleDegrees_ >= 360.0f)
            {
                cubeAngleDegrees_ -= 360.0f;
            }
            renderer_.DrawPyramid(cubeAngleDegrees_);
        }
    }
}
