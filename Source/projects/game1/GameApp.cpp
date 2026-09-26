#include "Engine/Engine.hpp"

#include <filesystem>
#include <memory>

namespace GamePyramid
{
    class GameApp final : public Engine::Core::Application
    {
    public:
        GameApp()
            : Engine::Core::Application(Engine::Core::ApplicationSpec{
                  .window = Engine::Platform::WindowSpec{.width = 1280, .height = 720, .title = "GamePyramid"},
                  .maxFrames = 0})
        {
        }

    protected:
        void OnInit() override
        {
            runtime_ = std::make_unique<Engine::Game::GameRuntime>();

            // Packaged build: projects/ sits next to the exe
            auto projectDir = std::filesystem::current_path() / "projects" / "game1";
#if defined(GAM300_ROOT_DIR)
            // Source tree the exe was built from (only exists on the build machine)
            const auto fromBuild = std::filesystem::path(GAM300_ROOT_DIR) / "projects" / "game1";
            if (std::filesystem::exists(fromBuild))
            {
                projectDir = fromBuild;
            }
#endif
            runtime_->LoadProject(Engine::Game::GameProject(projectDir));

            runtime_->Start();
        }

        void OnUpdate(double deltaSeconds) override { runtime_->Tick(deltaSeconds, GetWindow()); }

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
