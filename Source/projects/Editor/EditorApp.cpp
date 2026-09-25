#include "Engine/Engine.hpp"

#include "Editor/Commands/CommandStack.hpp"
#include "Editor/EditorCamera/EditorCamera.hpp"
#include "Editor/Gizmos/GizmoSystem.hpp"
#include "Editor/Panels/Console/ConsolePanel.hpp"
#include "Editor/Panels/GameView/GameViewPanel.hpp"
#include "Editor/Panels/Hierarchy/HierarchyPanel.hpp"
#include "Editor/Panels/Inspector/InspectorPanel.hpp"
#include "Editor/Panels/SceneView/SceneViewPanel.hpp"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#ifdef CreateWindow
#undef CreateWindow
#endif
#endif

#include <GL/gl.h>

#include <imgui/imgui.h>

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace Editor
{
    class EditorApp final : public Engine::Core::Application
    {
    public:
        EditorApp()
            : Engine::Core::Application(
                  Engine::Core::ApplicationSpec{
                      .window = Engine::Platform::WindowSpec{.width = 1280, .height = 720, .title = "Editor"},
                      .maxFrames = 0})
        {
        }

    protected:
        void OnInit() override
        {
            Engine::Core::Log(Engine::Core::LogLevel::Info, "Editor initialized");
            scene_ = std::make_unique<Engine::Scene::Scene>();

            panels_.sceneView = std::make_unique<Panels::SceneViewPanel>();
            panels_.gameView = std::make_unique<Panels::GameViewPanel>();
            panels_.hierarchy = std::make_unique<Panels::HierarchyPanel>();
            panels_.inspector = std::make_unique<Panels::InspectorPanel>();
            panels_.console = std::make_unique<Panels::ConsolePanel>();

            glClearColor(0.08f, 0.09f, 0.11f, 1.0f);
            glEnable(GL_DEPTH_TEST);

            imgui_.Init(GetWindow());
            RefreshProjectList();
        }

        void OnUpdate(double deltaSeconds) override
        {
            imgui_.BeginFrame(GetWindow(), deltaSeconds);

            if (ImGui::IsKeyPressed(ImGuiKey_F5, false))
            {
                if (isPlaying_)
                {
                    StopPlay();
                }
                else
                {
                    StartPlay();
                }
            }

            DrawProjectPickerUI();

            if (gameRuntime_ && isPlaying_)
            {
                gameRuntime_->Tick(deltaSeconds, GetWindow());
            }
            else
            {
                const int w = GetWindow().Width();
                const int h = GetWindow().Height();
                glViewport(0, 0, w, h);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }

            editorCamera_.Update(deltaSeconds);
            gizmos_.Update(deltaSeconds);

            panels_.sceneView->Draw();
            panels_.gameView->Draw();
            panels_.hierarchy->Draw();
            panels_.inspector->Draw();
            panels_.console->Draw();

            imgui_.EndFrame();
        }

        void OnShutdown() override
        {
            StopPlay();
            imgui_.Shutdown();
            Engine::Core::Log(Engine::Core::LogLevel::Info, "Editor shutdown");
        }

    private:
        static std::filesystem::path ProjectsRoot()
        {
#if defined(GAM300_ROOT_DIR)
            return std::filesystem::path(GAM300_ROOT_DIR) / "projects";
#else
            return std::filesystem::current_path() / "projects";
#endif
        }

        void RefreshProjectList()
        {
            availableProjectDirs_.clear();
            availableProjectNames_.clear();

            const auto root = ProjectsRoot();
            if (!std::filesystem::exists(root))
            {
                return;
            }

            for (const auto& entry : std::filesystem::directory_iterator(root))
            {
                if (!entry.is_directory())
                {
                    continue;
                }

                const auto dir = entry.path();
                const auto scene = dir / "Assets" / "Scenes" / "Default.scene";
                if (!std::filesystem::exists(scene))
                {
                    continue;
                }

                availableProjectDirs_.push_back(dir);
                availableProjectNames_.push_back(dir.filename().string());
            }

            if (availableProjectDirs_.empty())
            {
                selectedProjectIndex_ = 0;
                selectedProjectDir_.clear();
                return;
            }

            if (selectedProjectIndex_ < 0 || selectedProjectIndex_ >= static_cast<int>(availableProjectDirs_.size()))
            {
                selectedProjectIndex_ = 0;
            }

            selectedProjectDir_ = availableProjectDirs_[static_cast<size_t>(selectedProjectIndex_)];
        }

        void StartPlay()
        {
            if (availableProjectDirs_.empty())
            {
                RefreshProjectList();
            }

            if (availableProjectDirs_.empty())
            {
                return;
            }

            selectedProjectDir_ = availableProjectDirs_[static_cast<size_t>(selectedProjectIndex_)];

            gameRuntime_ = std::make_unique<Engine::Game::GameRuntime>();
            gameRuntime_->LoadProject(Engine::Game::GameProject(selectedProjectDir_));
            gameRuntime_->Start();
            isPlaying_ = true;
        }

        void StopPlay()
        {
            isPlaying_ = false;
            gameRuntime_.reset();
        }

        void DrawProjectPickerUI()
        {
            ImGui::SetNextWindowPos(ImVec2(12.0f, 12.0f), ImGuiCond_Once);
            ImGui::SetNextWindowSize(ImVec2(360.0f, 140.0f), ImGuiCond_Once);
            if (!ImGui::Begin("Game Project"))
            {
                ImGui::End();
                return;
            }

            if (ImGui::Button("Refresh"))
            {
                RefreshProjectList();
            }

            ImGui::SameLine();
            if (!availableProjectNames_.empty())
            {
                const char* preview = availableProjectNames_[static_cast<size_t>(selectedProjectIndex_)].c_str();
                if (ImGui::BeginCombo("Project", preview))
                {
                    for (int i = 0; i < static_cast<int>(availableProjectNames_.size()); ++i)
                    {
                        const bool isSelected = (i == selectedProjectIndex_);
                        if (ImGui::Selectable(availableProjectNames_[static_cast<size_t>(i)].c_str(), isSelected))
                        {
                            selectedProjectIndex_ = i;
                            selectedProjectDir_ = availableProjectDirs_[static_cast<size_t>(i)];
                        }
                        if (isSelected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
            }
            else
            {
                ImGui::TextUnformatted("No game projects found under /projects.");
            }

            if (isPlaying_)
            {
                if (ImGui::Button("Stop (F5)"))
                {
                    StopPlay();
                }
            }
            else
            {
                if (ImGui::Button("Play (F5)"))
                {
                    StartPlay();
                }
            }

            ImGui::Text("Selected: %s", selectedProjectDir_.empty() ? "" : selectedProjectDir_.string().c_str());

            ImGui::End();
        }

        struct PanelSet
        {
            std::unique_ptr<Panels::SceneViewPanel> sceneView;
            std::unique_ptr<Panels::GameViewPanel> gameView;
            std::unique_ptr<Panels::HierarchyPanel> hierarchy;
            std::unique_ptr<Panels::InspectorPanel> inspector;
            std::unique_ptr<Panels::ConsolePanel> console;
        };

        std::unique_ptr<Engine::Scene::Scene> scene_;
        Camera::EditorCamera editorCamera_;
        Gizmos::GizmoSystem gizmos_;
        Commands::CommandStack commands_;
        PanelSet panels_;
        Engine::ImGuiLayer::ImGuiLayer imgui_;
        std::unique_ptr<Engine::Game::GameRuntime> gameRuntime_;
        bool isPlaying_ = false;
        std::vector<std::filesystem::path> availableProjectDirs_;
        std::vector<std::string> availableProjectNames_;
        std::filesystem::path selectedProjectDir_;
        int selectedProjectIndex_ = 0;
    };
}

int main()
{
    Editor::EditorApp app;
    app.Run();
    return 0;
}
