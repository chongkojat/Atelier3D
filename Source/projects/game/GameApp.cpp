#include "Engine/Engine.hpp"

#include <memory>

namespace Game
{
    class GameApp final : public Engine::Core::Application
    {
    public:
        GameApp()
            : Engine::Core::Application(
                  Engine::Core::ApplicationSpec{
                      .window = Engine::Platform::WindowSpec{.width = 1280, .height = 720, .title = "Game"},
                      .maxFrames = 0})
        {
        }

    protected:
        void OnInit() override
        {
            runtime_ = std::make_unique<Engine::Game::GameRuntime>();
            runtime_->LoadProject(Engine::Game::GameProject(Engine::Game::GameProject::FindDefaultGameProjectDir()));
            runtime_->Start();
        }

        void OnUpdate(double deltaSeconds) override
        {
            runtime_->Tick(deltaSeconds, GetWindow());
        }

    private:
        std::unique_ptr<Engine::Game::GameRuntime> runtime_;
    };
}

int main()
{
    Game::GameApp app;
    app.Run();
    return 0;
}

