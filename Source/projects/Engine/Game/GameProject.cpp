#include "Engine/Game/GameProject.hpp"

#include <cstdlib>
#include <fstream>
#include <optional>
#include <string>

namespace Engine::Game
{
    namespace
    {
        std::optional<float> TryParseFloatAfterEquals(const std::string& token)
        {
            const auto eq = token.find('=');
            if (eq == std::string::npos || eq + 1 >= token.size())
            {
                return std::nullopt;
            }

            try
            {
                return std::stof(token.substr(eq + 1));
            }
            catch (...)
            {
                return std::nullopt;
            }
        }

        SceneDescription ParseSceneFile(const std::filesystem::path& scenePath)
        {
            SceneDescription desc{};

            std::ifstream file(scenePath);
            if (!file.is_open())
            {
                return desc;
            }

            std::string word;
            while (file >> word)
            {
                if (word == "Cube" || word == "cube")
                {
                    desc.hasCube = true;
                }
                else if (word == "Pyramid" || word == "pyramid")
                {
                    desc.hasPyramid = true;
                }
                else if (word.rfind("rotation_speed", 0) == 0)
                {
                    if (const auto value = TryParseFloatAfterEquals(word))
                    {
                        desc.cubeRotationSpeedDegreesPerSecond = *value;
                    }
                }
            }

            return desc;
        }

        std::optional<std::filesystem::path> TryFindGameProjectFrom(const std::filesystem::path& start)
        {
            std::filesystem::path current = start;
            for (int i = 0; i < 8; ++i)
            {
                const auto candidate = current / "projects" / "game";
                if (std::filesystem::exists(candidate))
                {
                    return candidate;
                }

                if (!current.has_parent_path())
                {
                    break;
                }
                current = current.parent_path();
            }
            return std::nullopt;
        }
    }

    GameProject::GameProject(std::filesystem::path projectDir) : projectDir_(std::move(projectDir)) {}

    const std::filesystem::path& GameProject::ProjectDir() const
    {
        return projectDir_;
    }

    SceneDescription GameProject::LoadDefaultScene() const
    {
        const auto scenePath = projectDir_ / "Assets" / "Scenes" / "Default.scene";
        return ParseSceneFile(scenePath);
    }

    std::filesystem::path GameProject::FindDefaultGameProjectDir()
    {
        if (const char* env = std::getenv("GAM300_GAME_PROJECT"))
        {
            const std::filesystem::path fromEnv = env;
            if (!fromEnv.empty() && std::filesystem::exists(fromEnv))
            {
                return fromEnv;
            }
        }

#if defined(GAM300_ROOT_DIR)
        {
            const auto fromBuild = std::filesystem::path(GAM300_ROOT_DIR) / "projects" / "game";
            if (std::filesystem::exists(fromBuild))
            {
                return fromBuild;
            }
        }
#endif

        if (const auto found = TryFindGameProjectFrom(std::filesystem::current_path()))
        {
            return *found;
        }

        return std::filesystem::path("projects") / "game";
    }
}
