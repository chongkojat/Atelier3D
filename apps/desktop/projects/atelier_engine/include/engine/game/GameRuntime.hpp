#pragma once

#include "engine/game/GameProject.hpp"
#include "engine/renderer/Renderer.hpp"

#include <memory>
#include <vector>

namespace Engine::Platform
{
    class Window;
}

namespace Engine::Game
{
    // Plays any game project: loads its start scene and draws every object in it
    class GameRuntime
    {
    public:
        GameRuntime();

        void LoadProject(GameProject project);
        // Returns false if no project is loaded and no default game project could be found
        bool Start();
        void Stop();
        bool IsRunning() const;

        const GameProject* Project() const;
        const SceneDescription& Scene() const;

        void Tick(double deltaSeconds, const Platform::Window& window);

    private:
        std::unique_ptr<GameProject> project_;
        SceneDescription scene_;
        std::vector<float> anglesDegrees_;
        Renderer::Renderer renderer_;
        bool running_ = false;
    };
}
