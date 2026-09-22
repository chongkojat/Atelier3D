#pragma once

#include <filesystem>

namespace Engine::Game
{
    struct SceneDescription
    {
        bool hasCube = false;
        bool hasPyramid = false;
        float cubeRotationSpeedDegreesPerSecond = 60.0f;
    };

    class GameProject
    {
    public:
        explicit GameProject(std::filesystem::path projectDir);

        const std::filesystem::path& ProjectDir() const;
        SceneDescription LoadDefaultScene() const;

        static std::filesystem::path FindDefaultGameProjectDir();

    private:
        std::filesystem::path projectDir_;
    };
}
