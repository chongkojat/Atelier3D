#include "engine/Engine.hpp"

#include "editor/camera/EditorCamera.hpp"
#include "editor/commands/CommandStack.hpp"
#include "editor/gizmos/GizmoSystem.hpp"
#include "editor/panels/console/ConsolePanel.hpp"
#include "editor/panels/gameview/GameViewPanel.hpp"
#include "editor/panels/hierarchy/HierarchyPanel.hpp"
#include "editor/panels/inspector/InspectorPanel.hpp"
#include "editor/panels/sceneview/SceneViewPanel.hpp"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#ifdef CreateWindow
#undef CreateWindow
#endif
#endif

#include <GL/gl.h>

#include <imgui/imgui.h>

#include <array>
#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace Editor
{
    class EditorApp final : public Engine::Core::Application
    {
    public:
        EditorApp()
            : Engine::Core::Application(Engine::Core::ApplicationSpec{
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
        void RefreshProjectList()
        {
            availableProjectDirs_.clear();
            availableProjectNames_.clear();

            for (const auto& dir : Engine::Game::GameProject::FindAll(Engine::Game::GameProject::ProjectsRoot()))
            {
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

        void SelectProject(const std::filesystem::path& dir)
        {
            for (std::size_t i = 0; i < availableProjectDirs_.size(); ++i)
            {
                if (availableProjectDirs_[i] == dir)
                {
                    selectedProjectIndex_ = static_cast<int>(i);
                    selectedProjectDir_ = dir;
                }
            }
        }

        void CreateGame()
        {
            try
            {
                const auto project = Engine::Game::GameProject::Create(
                    Engine::Game::GameProject::ProjectsRoot(), newGameName_.data(), kTemplates[newGameTemplate_]);
                createMessage_ = "Created " + project.ProjectDir().string();
                createFailed_ = false;
                Engine::Core::Log(Engine::Core::LogLevel::Info, createMessage_);

                RefreshProjectList();
                SelectProject(project.ProjectDir());
                newGameName_.fill('\0');
            }
            catch (const std::exception& e)
            {
                createMessage_ = e.what();
                createFailed_ = true;
                Engine::Core::Log(Engine::Core::LogLevel::Error, createMessage_);
            }
        }

        void DrawNewGameUI()
        {
            ImGui::SeparatorText("New Game");
            ImGui::InputText("Name", newGameName_.data(), newGameName_.size());

            if (ImGui::BeginCombo("Template", Engine::Game::ToString(kTemplates[newGameTemplate_])))
            {
                for (int i = 0; i < static_cast<int>(kTemplates.size()); ++i)
                {
                    if (ImGui::Selectable(Engine::Game::ToString(kTemplates[i]), i == newGameTemplate_))
                    {
                        newGameTemplate_ = i;
                    }
                }
                ImGui::EndCombo();
            }

            if (ImGui::Button("Create"))
            {
                CreateGame();
            }

            if (!createMessage_.empty())
            {
                const ImVec4 color = createFailed_ ? ImVec4(1.0f, 0.4f, 0.4f, 1.0f) : ImVec4(0.4f, 1.0f, 0.4f, 1.0f);
                ImGui::TextColored(color, "%s", createMessage_.c_str());
            }
        }

        void DrawProjectPickerUI()
        {
            ImGui::SetNextWindowPos(ImVec2(12.0f, 12.0f), ImGuiCond_Once);
            ImGui::SetNextWindowSize(ImVec2(420.0f, 260.0f), ImGuiCond_Once);
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
                ImGui::TextUnformatted("No game projects yet. Create one below.");
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

            DrawNewGameUI();

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

        static constexpr std::array kTemplates{Engine::Game::GameTemplate::Empty, Engine::Game::GameTemplate::Cube,
                                               Engine::Game::GameTemplate::Pyramid};
        std::array<char, 65> newGameName_{};
        int newGameTemplate_ = 1;
        std::string createMessage_;
        bool createFailed_ = false;
    };

    std::optional<Engine::Game::GameTemplate> ParseTemplate(std::string_view name)
    {
        for (const auto gameTemplate :
             {Engine::Game::GameTemplate::Empty, Engine::Game::GameTemplate::Cube, Engine::Game::GameTemplate::Pyramid})
        {
            if (name == Engine::Game::ToString(gameTemplate))
            {
                return gameTemplate;
            }
        }
        return std::nullopt;
    }

    // AtelierEditor --new-game <name> [--template empty|cube|pyramid] [--projects-dir <dir>]
    // Creates a game project without opening a window; returns the process exit code
    int RunNewGameCommand(int argc, char** argv)
    {
        std::string name;
        auto gameTemplate = Engine::Game::GameTemplate::Cube;
        auto projectsDir = Engine::Game::GameProject::ProjectsRoot();

        for (int i = 1; i < argc; ++i)
        {
            const std::string_view arg = argv[i];
            const bool hasValue = i + 1 < argc;
            if (arg == "--new-game" && hasValue)
            {
                name = argv[++i];
            }
            else if (arg == "--template" && hasValue)
            {
                const auto parsed = ParseTemplate(argv[++i]);
                if (!parsed)
                {
                    std::cerr << "Unknown template '" << argv[i] << "' (expected empty, cube or pyramid)\n";
                    return 1;
                }
                gameTemplate = *parsed;
            }
            else if (arg == "--projects-dir" && hasValue)
            {
                projectsDir = argv[++i];
            }
            else
            {
                std::cerr << "Usage: AtelierEditor --new-game <name> [--template empty|cube|pyramid] "
                             "[--projects-dir <dir>]\n";
                return 1;
            }
        }

        try
        {
            const auto project = Engine::Game::GameProject::Create(projectsDir, name, gameTemplate);
            std::cout << "Created game project " << project.ProjectDir().string() << "\n";
            return 0;
        }
        catch (const std::exception& e)
        {
            std::cerr << "ERROR: " << e.what() << "\n";
            return 1;
        }
    }
}

int main(int argc, char** argv)
{
    // Any arguments mean a command-line action; no arguments open the editor
    if (argc > 1)
    {
        return Editor::RunNewGameCommand(argc, argv);
    }

    Editor::EditorApp app;
    app.Run();
    return 0;
}
