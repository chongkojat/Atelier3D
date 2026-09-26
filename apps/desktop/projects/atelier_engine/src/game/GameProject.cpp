#include "engine/game/GameProject.hpp"

#include "engine/platform/Paths.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <optional>
#include <sstream>
#include <stdexcept>

namespace Engine::Game
{
    namespace
    {
        constexpr const char* kConfigFile = "configs/game.cfg";
        constexpr const char* kMainScene = "data/scenes/main.scene";

        std::string ToLower(std::string text)
        {
            std::transform(text.begin(), text.end(), text.begin(),
                           [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            return text;
        }

        std::string Trim(const std::string& text)
        {
            const auto first = text.find_first_not_of(" \t\r\n");
            if (first == std::string::npos)
            {
                return {};
            }
            const auto last = text.find_last_not_of(" \t\r\n");
            return text.substr(first, last - first + 1);
        }

        // Drops everything from '#' to the end of the line
        std::string StripComment(const std::string& line)
        {
            return line.substr(0, line.find('#'));
        }

        std::optional<float> TryParseFloat(const std::string& text)
        {
            try
            {
                std::size_t used = 0;
                const float value = std::stof(text, &used);
                if (used != text.size())
                {
                    return std::nullopt;
                }
                return value;
            }
            catch (...)
            {
                return std::nullopt;
            }
        }

        std::optional<int> TryParseInt(const std::string& text)
        {
            try
            {
                std::size_t used = 0;
                const int value = std::stoi(text, &used);
                if (used != text.size())
                {
                    return std::nullopt;
                }
                return value;
            }
            catch (...)
            {
                return std::nullopt;
            }
        }

        std::optional<Shape> TryParseShape(const std::string& word)
        {
            const auto lower = ToLower(word);
            if (lower == "cube")
            {
                return Shape::Cube;
            }
            if (lower == "pyramid")
            {
                return Shape::Pyramid;
            }
            return std::nullopt;
        }

        // A shape word starts a new object; key=value words set properties of the latest object
        SceneDescription ParseScene(std::istream& input)
        {
            SceneDescription scene;

            std::string line;
            while (std::getline(input, line))
            {
                std::istringstream words(StripComment(line));
                std::string word;
                while (words >> word)
                {
                    if (const auto shape = TryParseShape(word))
                    {
                        scene.objects.push_back(SceneObject{.shape = *shape});
                        continue;
                    }

                    const auto eq = word.find('=');
                    if (eq == std::string::npos || scene.objects.empty())
                    {
                        continue;
                    }

                    const auto key = ToLower(word.substr(0, eq));
                    const auto value = TryParseFloat(word.substr(eq + 1));
                    if (!value)
                    {
                        continue;
                    }

                    SceneObject& object = scene.objects.back();
                    if (key == "x")
                    {
                        object.x = *value;
                    }
                    else if (key == "y")
                    {
                        object.y = *value;
                    }
                    else if (key == "z")
                    {
                        object.z = *value;
                    }
                    else if (key == "rotation_speed")
                    {
                        object.rotationSpeedDegreesPerSecond = *value;
                    }
                }
            }

            return scene;
        }

        void ParseSettings(std::istream& input, GameSettings& settings)
        {
            std::string line;
            while (std::getline(input, line))
            {
                line = StripComment(line);
                const auto eq = line.find('=');
                if (eq == std::string::npos)
                {
                    continue;
                }

                const auto key = ToLower(Trim(line.substr(0, eq)));
                const auto value = Trim(line.substr(eq + 1));
                if (value.empty())
                {
                    continue;
                }

                if (key == "name")
                {
                    settings.name = value;
                }
                else if (key == "start_scene")
                {
                    settings.startScene = value;
                }
                else if (key == "window_width" || key == "window_height")
                {
                    const auto number = TryParseInt(value);
                    if (!number || *number <= 0)
                    {
                        continue;
                    }
                    (key == "window_width" ? settings.windowWidth : settings.windowHeight) = *number;
                }
            }
        }

        std::string SceneFor(GameTemplate gameTemplate)
        {
            std::string scene = "# One object per line: <cube|pyramid> [x=..] [y=..] [z=..] [rotation_speed=..]\n";
            switch (gameTemplate)
            {
            case GameTemplate::Empty: break;
            case GameTemplate::Cube: scene += "cube rotation_speed=60\n"; break;
            case GameTemplate::Pyramid: scene += "pyramid rotation_speed=60\n"; break;
            }
            return scene;
        }

        void WriteFile(const std::filesystem::path& path, const std::string& contents)
        {
            std::ofstream file(path, std::ios::binary);
            if (!file)
            {
                throw std::runtime_error("Could not write " + path.string());
            }
            file << contents;
        }
    }

    GameProject::GameProject(std::filesystem::path projectDir) : projectDir_(std::move(projectDir))
    {
        settings_.name = projectDir_.filename().string();

        std::ifstream config(projectDir_ / kConfigFile);
        if (config)
        {
            ParseSettings(config, settings_);
        }
    }

    const std::filesystem::path& GameProject::ProjectDir() const
    {
        return projectDir_;
    }

    const GameSettings& GameProject::Settings() const
    {
        return settings_;
    }

    SceneDescription GameProject::LoadStartScene() const
    {
        return LoadScene(settings_.startScene);
    }

    SceneDescription GameProject::LoadScene(const std::filesystem::path& relativePath) const
    {
        std::ifstream file(projectDir_ / relativePath);
        if (!file)
        {
            return {};
        }
        return ParseScene(file);
    }

    bool GameProject::IsGameProject(const std::filesystem::path& dir)
    {
        std::error_code ec;
        return std::filesystem::is_regular_file(dir / kConfigFile, ec);
    }

    std::vector<std::filesystem::path> GameProject::FindAll(const std::filesystem::path& projectsRoot)
    {
        std::vector<std::filesystem::path> projects;

        std::error_code ec;
        for (const auto& entry : std::filesystem::directory_iterator(projectsRoot, ec))
        {
            if (entry.is_directory() && IsGameProject(entry.path()))
            {
                projects.push_back(entry.path());
            }
        }

        std::sort(projects.begin(), projects.end());
        return projects;
    }

    std::filesystem::path GameProject::ProjectsRoot()
    {
        // Create() makes the folder with the first game, so it may not exist yet
        return Platform::ExecutableDir() / "projects";
    }

    std::filesystem::path GameProject::FindDefaultGameProjectDir()
    {
        if (const char* env = std::getenv("ATELIER_GAME_PROJECT"))
        {
            const std::filesystem::path fromEnv = env;
            if (!fromEnv.empty() && std::filesystem::is_directory(fromEnv))
            {
                return fromEnv;
            }
        }

        const auto projects = FindAll(ProjectsRoot());
        return projects.empty() ? std::filesystem::path{} : projects.front();
    }

    bool GameProject::IsValidName(std::string_view name)
    {
        if (name.empty() || name.size() > 64 || !std::isalnum(static_cast<unsigned char>(name.front())))
        {
            return false;
        }

        return std::all_of(name.begin(), name.end(),
                           [](char c) { return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '-'; });
    }

    GameProject GameProject::Create(const std::filesystem::path& projectsRoot, const std::string& name,
                                    GameTemplate gameTemplate)
    {
        if (!IsValidName(name))
        {
            throw std::runtime_error("Invalid game name '" + name +
                                     "': use 1-64 letters, digits, '_' or '-', starting with a letter or digit");
        }

        const auto dir = projectsRoot / name;
        if (std::filesystem::exists(dir))
        {
            throw std::runtime_error("A folder named '" + name + "' already exists in " + projectsRoot.string());
        }

        // .gitkeep keeps the empty folders when the game is committed
        for (const char* folder :
             {"assets/models", "assets/textures", "assets/materials", "assets/shaders", "assets/audio", "scripts"})
        {
            std::filesystem::create_directories(dir / folder);
            WriteFile(dir / folder / ".gitkeep", "");
        }
        std::filesystem::create_directories(dir / "configs");
        std::filesystem::create_directories(dir / "data" / "scenes");

        WriteFile(dir / kConfigFile, "# Atelier3D game settings\n"
                                     "name=" +
                                         name +
                                         "\n"
                                         "window_width=1280\n"
                                         "window_height=720\n"
                                         "start_scene=" +
                                         std::string(kMainScene) + "\n");
        WriteFile(dir / kMainScene, SceneFor(gameTemplate));

        return GameProject(dir);
    }

    const char* ToString(GameTemplate gameTemplate)
    {
        switch (gameTemplate)
        {
        case GameTemplate::Empty: return "empty";
        case GameTemplate::Cube: return "cube";
        case GameTemplate::Pyramid: return "pyramid";
        }
        return "empty";
    }
}
