#include "Engine/Engine.hpp"

#include <filesystem>
#include <memory>

namespace GamePyramid
{
    class GameApp final : public Engine::Core::Application
    {
    public:
        GameApp()
            : Engine::Core::Application(
                  Engine::Core::ApplicationSpec{
                      .window = Engine::Platform::WindowSpec{.width = 1280, .height = 720, .title = "GamePyramid"},
                      .maxFrames = 0})
        {
        }

    protected:
        void OnInit() override
        {
            runtime_ = std::make_unique<Engine::Game::GameRuntime>();

#if defined(GAM300_ROOT_DIR)
            const auto projectDir = std::filesystem::path(GAM300_ROOT_DIR) / "projects" / "game_pyramid";
            runtime_->LoadProject(Engine::Game::GameProject(projectDir));
#endif

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
    GamePyramid::GameApp app;
    app.Run();
    return 0;
}

