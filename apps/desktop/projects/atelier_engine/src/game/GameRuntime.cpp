#include "engine/game/GameRuntime.hpp"

#include "engine/core/Log.hpp"
#include "engine/platform/Window.hpp"

#include <cmath>

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

    bool GameRuntime::Start()
    {
        if (!project_)
        {
            const auto dir = GameProject::FindDefaultGameProjectDir();
            if (dir.empty())
            {
                Core::Log(Core::LogLevel::Error, "No game project found in " + GameProject::ProjectsRoot().string());
                return false;
            }
            LoadProject(GameProject(dir));
        }

        scene_ = project_->LoadStartScene();
        anglesDegrees_.assign(scene_.objects.size(), 0.0f);
        running_ = true;

        Core::Log(Core::LogLevel::Info, "Playing '" + project_->Settings().name + "' (" +
                                            std::to_string(scene_.objects.size()) + " objects)");
        return true;
    }

    void GameRuntime::Stop()
    {
        running_ = false;
    }

    bool GameRuntime::IsRunning() const
    {
        return running_;
    }

    const GameProject* GameRuntime::Project() const
    {
        return project_.get();
    }

    const SceneDescription& GameRuntime::Scene() const
    {
        return scene_;
    }

    void GameRuntime::Tick(double deltaSeconds, const Platform::Window& window)
    {
        renderer_.BeginFrame(window.Width(), window.Height());
        renderer_.Clear();

        if (!running_)
        {
            return;
        }

        for (std::size_t i = 0; i < scene_.objects.size(); ++i)
        {
            const SceneObject& object = scene_.objects[i];
            float& angle = anglesDegrees_[i];
            angle = std::fmod(angle + static_cast<float>(deltaSeconds) * object.rotationSpeedDegreesPerSecond, 360.0f);

            switch (object.shape)
            {
            case Shape::Cube: renderer_.DrawCube(angle, object.x, object.y, object.z); break;
            case Shape::Pyramid: renderer_.DrawPyramid(angle, object.x, object.y, object.z); break;
            }
        }
    }
}
