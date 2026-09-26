#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace Engine::Game
{
    enum class Shape : std::uint8_t
    {
        Cube,
        Pyramid
    };

    // One object in a scene file: "<shape> [x=..] [y=..] [z=..] [rotation_speed=..]"
    struct SceneObject
    {
        Shape shape = Shape::Cube;
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float rotationSpeedDegreesPerSecond = 60.0f;
    };

    struct SceneDescription
    {
        std::vector<SceneObject> objects;
    };

    // configs/game.cfg of a game project
    struct GameSettings
    {
        std::string name;
        int windowWidth = 1280;
        int windowHeight = 720;
        std::string startScene = "data/scenes/main.scene";
    };

    // Starting content for a newly created game
    enum class GameTemplate : std::uint8_t
    {
        Empty,
        Cube,
        Pyramid
    };

    // A game is a folder of data, so new games need no C++ code or build changes:
    //   <name>/configs/game.cfg       settings (marks the folder as a game project)
    //   <name>/data/scenes/*.scene    scenes
    //   <name>/assets/, <name>/scripts/
    class GameProject
    {
    public:
        // Reads configs/game.cfg; missing values keep their defaults and the name defaults to the folder name
        explicit GameProject(std::filesystem::path projectDir);

        const std::filesystem::path& ProjectDir() const;
        const GameSettings& Settings() const;

        SceneDescription LoadStartScene() const;
        // Path relative to the project folder, e.g. "data/scenes/main.scene"
        SceneDescription LoadScene(const std::filesystem::path& relativePath) const;

        static bool IsGameProject(const std::filesystem::path& dir);
        // Game projects directly inside projectsRoot, sorted by folder name
        static std::vector<std::filesystem::path> FindAll(const std::filesystem::path& projectsRoot);

        // Where game projects live: projects/ in the folder containing the running exe
        static std::filesystem::path ProjectsRoot();
        // ATELIER_GAME_PROJECT if it points at a folder, else the first game in ProjectsRoot(), else an empty path
        static std::filesystem::path FindDefaultGameProjectDir();

        // 1-64 characters: letters, digits, '_' or '-', starting with a letter or digit
        static bool IsValidName(std::string_view name);

        // Creates projectsRoot/<name> with the standard folders, configs/game.cfg and data/scenes/main.scene.
        // Throws std::runtime_error if the name is invalid or the folder already exists.
        static GameProject Create(const std::filesystem::path& projectsRoot, const std::string& name,
                                  GameTemplate gameTemplate);

    private:
        std::filesystem::path projectDir_;
        GameSettings settings_;
    };

    const char* ToString(GameTemplate gameTemplate);
}
