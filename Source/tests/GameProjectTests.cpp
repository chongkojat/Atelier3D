#include "Engine/Game/GameProject.hpp"

#include <gtest/gtest.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>

using Engine::Game::GameProject;
using Engine::Game::SceneDescription;

namespace
{
    void SetEnv(const char* name, const char* value)
    {
#ifdef _WIN32
        _putenv_s(name, value);
#else
        setenv(name, value, 1);
#endif
    }

    void UnsetEnv(const char* name)
    {
#ifdef _WIN32
        _putenv_s(name, "");
#else
        unsetenv(name);
#endif
    }

    // Creates a throwaway game project folder for each test
    class GameProjectTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            const auto* info = ::testing::UnitTest::GetInstance()->current_test_info();
            projectDir_ = std::filesystem::temp_directory_path() / "gam300_tests" / info->name();
            std::filesystem::remove_all(projectDir_);
            std::filesystem::create_directories(projectDir_);
        }

        void TearDown() override { std::filesystem::remove_all(projectDir_); }

        void WriteScene(const std::string& contents) const
        {
            const auto scenes = projectDir_ / "Assets" / "Scenes";
            std::filesystem::create_directories(scenes);
            std::ofstream(scenes / "Default.scene") << contents;
        }

        SceneDescription Load() const { return GameProject(projectDir_).LoadDefaultScene(); }

        std::filesystem::path projectDir_;
    };
}

TEST_F(GameProjectTest, KeepsProjectDir)
{
    EXPECT_EQ(GameProject(projectDir_).ProjectDir(), projectDir_);
}

TEST_F(GameProjectTest, MissingSceneFileGivesDefaults)
{
    const SceneDescription scene = Load();
    EXPECT_FALSE(scene.hasCube);
    EXPECT_FALSE(scene.hasPyramid);
    EXPECT_FLOAT_EQ(scene.cubeRotationSpeedDegreesPerSecond, 60.0f);
}

TEST_F(GameProjectTest, ParsesCubeScene)
{
    WriteScene("Cube rotation_speed=45\n");

    const SceneDescription scene = Load();
    EXPECT_TRUE(scene.hasCube);
    EXPECT_FALSE(scene.hasPyramid);
    EXPECT_FLOAT_EQ(scene.cubeRotationSpeedDegreesPerSecond, 45.0f);
}

TEST_F(GameProjectTest, ParsesPyramidScene)
{
    WriteScene("Pyramid rotation_speed=90.5\n");

    const SceneDescription scene = Load();
    EXPECT_FALSE(scene.hasCube);
    EXPECT_TRUE(scene.hasPyramid);
    EXPECT_FLOAT_EQ(scene.cubeRotationSpeedDegreesPerSecond, 90.5f);
}

TEST_F(GameProjectTest, LowercaseShapeNamesAreAccepted)
{
    WriteScene("cube pyramid\n");

    const SceneDescription scene = Load();
    EXPECT_TRUE(scene.hasCube);
    EXPECT_TRUE(scene.hasPyramid);
}

TEST_F(GameProjectTest, InvalidRotationSpeedKeepsDefault)
{
    WriteScene("Cube rotation_speed=fast\n");

    const SceneDescription scene = Load();
    EXPECT_TRUE(scene.hasCube);
    EXPECT_FLOAT_EQ(scene.cubeRotationSpeedDegreesPerSecond, 60.0f);
}

TEST_F(GameProjectTest, EmptyRotationSpeedKeepsDefault)
{
    WriteScene("Cube rotation_speed=\n");

    EXPECT_FLOAT_EQ(Load().cubeRotationSpeedDegreesPerSecond, 60.0f);
}

TEST_F(GameProjectTest, UnknownWordsAreIgnored)
{
    WriteScene("Sphere Light Camera\n");

    const SceneDescription scene = Load();
    EXPECT_FALSE(scene.hasCube);
    EXPECT_FALSE(scene.hasPyramid);
}

TEST_F(GameProjectTest, EnvironmentVariableOverridesProjectDir)
{
    SetEnv("GAM300_GAME_PROJECT", projectDir_.string().c_str());
    const auto found = GameProject::FindDefaultGameProjectDir();
    UnsetEnv("GAM300_GAME_PROJECT");

    EXPECT_EQ(found, projectDir_);
}

TEST_F(GameProjectTest, NonexistentEnvironmentPathIsIgnored)
{
    const auto missing = projectDir_ / "does_not_exist";
    SetEnv("GAM300_GAME_PROJECT", missing.string().c_str());
    const auto found = GameProject::FindDefaultGameProjectDir();
    UnsetEnv("GAM300_GAME_PROJECT");

    EXPECT_NE(found, missing);
}
