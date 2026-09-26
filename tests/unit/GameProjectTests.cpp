#include "engine/game/GameProject.hpp"
#include "engine/platform/Paths.hpp"

#include <gtest/gtest.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

using Engine::Game::GameProject;
using Engine::Game::GameTemplate;
using Engine::Game::SceneDescription;
using Engine::Game::Shape;

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

    // Gives each test an empty projects folder containing an empty "mygame" folder
    class GameProjectTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            const auto* info = ::testing::UnitTest::GetInstance()->current_test_info();
            root_ = std::filesystem::temp_directory_path() / "atelier_tests" / info->name();
            std::filesystem::remove_all(root_);
            projectDir_ = root_ / "mygame";
            std::filesystem::create_directories(projectDir_);
        }

        void TearDown() override { std::filesystem::remove_all(root_); }

        void WriteFile(const std::filesystem::path& relativePath, const std::string& contents) const
        {
            const auto path = projectDir_ / relativePath;
            std::filesystem::create_directories(path.parent_path());
            std::ofstream(path) << contents;
        }

        void WriteConfig(const std::string& contents) const { WriteFile("configs/game.cfg", contents); }
        void WriteScene(const std::string& contents) const { WriteFile("data/scenes/main.scene", contents); }

        SceneDescription Load() const { return GameProject(projectDir_).LoadStartScene(); }

        std::filesystem::path root_;
        std::filesystem::path projectDir_;
    };
}

// --- Settings (configs/game.cfg) ---

TEST_F(GameProjectTest, KeepsProjectDir)
{
    EXPECT_EQ(GameProject(projectDir_).ProjectDir(), projectDir_);
}

TEST_F(GameProjectTest, MissingConfigUsesFolderNameAndDefaults)
{
    const GameProject project(projectDir_);
    const auto& settings = project.Settings();
    EXPECT_EQ(settings.name, "mygame");
    EXPECT_EQ(settings.windowWidth, 1280);
    EXPECT_EQ(settings.windowHeight, 720);
    EXPECT_EQ(settings.startScene, "data/scenes/main.scene");
}

TEST_F(GameProjectTest, ParsesConfig)
{
    WriteConfig(
        "# comment\nname = My Game \nwindow_width=800\nwindow_height=600\nstart_scene=data/scenes/intro.scene\n");

    const GameProject project(projectDir_);
    const auto& settings = project.Settings();
    EXPECT_EQ(settings.name, "My Game");
    EXPECT_EQ(settings.windowWidth, 800);
    EXPECT_EQ(settings.windowHeight, 600);
    EXPECT_EQ(settings.startScene, "data/scenes/intro.scene");
}

TEST_F(GameProjectTest, InvalidConfigNumbersKeepDefaults)
{
    WriteConfig("window_width=wide\nwindow_height=-5\n");

    const GameProject project(projectDir_);
    const auto& settings = project.Settings();
    EXPECT_EQ(settings.windowWidth, 1280);
    EXPECT_EQ(settings.windowHeight, 720);
}

TEST_F(GameProjectTest, StartSceneFromConfigIsLoaded)
{
    WriteConfig("start_scene=data/scenes/other.scene\n");
    WriteFile("data/scenes/other.scene", "pyramid\n");

    const SceneDescription scene = Load();
    ASSERT_EQ(scene.objects.size(), 1u);
    EXPECT_EQ(scene.objects[0].shape, Shape::Pyramid);
}

// --- Scenes ---

TEST_F(GameProjectTest, MissingSceneFileGivesEmptyScene)
{
    EXPECT_TRUE(Load().objects.empty());
}

TEST_F(GameProjectTest, ParsesCubeScene)
{
    WriteScene("Cube rotation_speed=45\n");

    const SceneDescription scene = Load();
    ASSERT_EQ(scene.objects.size(), 1u);
    EXPECT_EQ(scene.objects[0].shape, Shape::Cube);
    EXPECT_FLOAT_EQ(scene.objects[0].rotationSpeedDegreesPerSecond, 45.0f);
}

TEST_F(GameProjectTest, ParsesSeveralObjectsWithPositions)
{
    WriteScene("cube x=-1.5 rotation_speed=30\npyramid x=1.5 y=0.25 z=-2 rotation_speed=90.5\n");

    const SceneDescription scene = Load();
    ASSERT_EQ(scene.objects.size(), 2u);
    EXPECT_EQ(scene.objects[0].shape, Shape::Cube);
    EXPECT_FLOAT_EQ(scene.objects[0].x, -1.5f);
    EXPECT_FLOAT_EQ(scene.objects[0].rotationSpeedDegreesPerSecond, 30.0f);
    EXPECT_EQ(scene.objects[1].shape, Shape::Pyramid);
    EXPECT_FLOAT_EQ(scene.objects[1].x, 1.5f);
    EXPECT_FLOAT_EQ(scene.objects[1].y, 0.25f);
    EXPECT_FLOAT_EQ(scene.objects[1].z, -2.0f);
    EXPECT_FLOAT_EQ(scene.objects[1].rotationSpeedDegreesPerSecond, 90.5f);
}

TEST_F(GameProjectTest, ShapeNamesAreCaseInsensitive)
{
    WriteScene("CUBE Pyramid\n");

    const SceneDescription scene = Load();
    ASSERT_EQ(scene.objects.size(), 2u);
    EXPECT_EQ(scene.objects[0].shape, Shape::Cube);
    EXPECT_EQ(scene.objects[1].shape, Shape::Pyramid);
}

TEST_F(GameProjectTest, CommentsAreIgnored)
{
    WriteScene("# cube\ncube # pyramid\n");

    EXPECT_EQ(Load().objects.size(), 1u);
}

TEST_F(GameProjectTest, InvalidRotationSpeedKeepsDefault)
{
    WriteScene("cube rotation_speed=fast\ncube rotation_speed=\ncube rotation_speed=5x\n");

    const SceneDescription scene = Load();
    ASSERT_EQ(scene.objects.size(), 3u);
    for (const auto& object : scene.objects)
    {
        EXPECT_FLOAT_EQ(object.rotationSpeedDegreesPerSecond, 60.0f);
    }
}

TEST_F(GameProjectTest, PropertiesBeforeAnyShapeAreIgnored)
{
    WriteScene("rotation_speed=10 cube\n");

    const SceneDescription scene = Load();
    ASSERT_EQ(scene.objects.size(), 1u);
    EXPECT_FLOAT_EQ(scene.objects[0].rotationSpeedDegreesPerSecond, 60.0f);
}

TEST_F(GameProjectTest, UnknownWordsAreIgnored)
{
    WriteScene("Sphere Light Camera\n");

    EXPECT_TRUE(Load().objects.empty());
}

// --- Creating games ---

TEST(GameProjectNameTest, AcceptsValidNames)
{
    EXPECT_TRUE(GameProject::IsValidName("my_game"));
    EXPECT_TRUE(GameProject::IsValidName("Level-2"));
    EXPECT_TRUE(GameProject::IsValidName("a"));
    EXPECT_TRUE(GameProject::IsValidName(std::string(64, 'x')));
}

TEST(GameProjectNameTest, RejectsInvalidNames)
{
    EXPECT_FALSE(GameProject::IsValidName(""));
    EXPECT_FALSE(GameProject::IsValidName("_hidden"));
    EXPECT_FALSE(GameProject::IsValidName("-flag"));
    EXPECT_FALSE(GameProject::IsValidName("my game"));
    EXPECT_FALSE(GameProject::IsValidName("../escape"));
    EXPECT_FALSE(GameProject::IsValidName("a/b"));
    EXPECT_FALSE(GameProject::IsValidName(std::string(65, 'x')));
}

TEST_F(GameProjectTest, CreateMakesStandardLayout)
{
    const auto project = GameProject::Create(root_, "newgame", GameTemplate::Cube);
    const auto dir = root_ / "newgame";

    EXPECT_EQ(project.ProjectDir(), dir);
    EXPECT_TRUE(GameProject::IsGameProject(dir));
    EXPECT_EQ(project.Settings().name, "newgame");
    for (const char* folder : {"assets/models", "assets/textures", "assets/materials", "assets/shaders", "assets/audio",
                               "scripts", "data/scenes"})
    {
        EXPECT_TRUE(std::filesystem::is_directory(dir / folder)) << folder;
    }
    EXPECT_TRUE(std::filesystem::exists(dir / "assets/models/.gitkeep"));
}

TEST_F(GameProjectTest, CreateUsesTemplateForStartScene)
{
    const auto cube = GameProject::Create(root_, "cube_game", GameTemplate::Cube).LoadStartScene();
    ASSERT_EQ(cube.objects.size(), 1u);
    EXPECT_EQ(cube.objects[0].shape, Shape::Cube);

    const auto pyramid = GameProject::Create(root_, "pyramid_game", GameTemplate::Pyramid).LoadStartScene();
    ASSERT_EQ(pyramid.objects.size(), 1u);
    EXPECT_EQ(pyramid.objects[0].shape, Shape::Pyramid);

    EXPECT_TRUE(GameProject::Create(root_, "empty_game", GameTemplate::Empty).LoadStartScene().objects.empty());
}

TEST_F(GameProjectTest, CreateRejectsInvalidName)
{
    EXPECT_THROW(GameProject::Create(root_, "../escape", GameTemplate::Cube), std::runtime_error);
    EXPECT_FALSE(std::filesystem::exists(root_.parent_path() / "escape"));
}

TEST_F(GameProjectTest, CreateRejectsExistingFolder)
{
    EXPECT_THROW(GameProject::Create(root_, "mygame", GameTemplate::Cube), std::runtime_error);
}

// --- Finding games ---

TEST(GameProjectRootTest, GamesLiveInProjectsFolderNextToTheExecutable)
{
    const auto exeDir = Engine::Platform::ExecutableDir();
    EXPECT_EQ(GameProject::ProjectsRoot(), exeDir / "projects");

    // ExecutableDir() is the folder this test binary is in
    bool foundTestExe = false;
    for (const auto& entry : std::filesystem::directory_iterator(exeDir))
    {
        foundTestExe |= entry.path().stem() == "AtelierTests";
    }
    EXPECT_TRUE(foundTestExe) << exeDir;
}

TEST_F(GameProjectTest, CreateMakesMissingProjectsFolder)
{
    const auto projectsDir = root_ / "projects"; // does not exist yet
    GameProject::Create(projectsDir, "first_game", GameTemplate::Empty);

    EXPECT_TRUE(GameProject::IsGameProject(projectsDir / "first_game"));
}

TEST_F(GameProjectTest, FindAllListsOnlyGameProjectsSorted)
{
    GameProject::Create(root_, "zeta", GameTemplate::Empty);
    GameProject::Create(root_, "alpha", GameTemplate::Empty);
    std::filesystem::create_directories(root_ / "atelier_engine" / "src"); // not a game project
    // "mygame" has no configs/game.cfg, so it is not a game project either

    const auto found = GameProject::FindAll(root_);
    ASSERT_EQ(found.size(), 2u);
    EXPECT_EQ(found[0].filename(), "alpha");
    EXPECT_EQ(found[1].filename(), "zeta");
}

TEST_F(GameProjectTest, FindAllOnMissingFolderIsEmpty)
{
    EXPECT_TRUE(GameProject::FindAll(root_ / "does_not_exist").empty());
}

TEST_F(GameProjectTest, EnvironmentVariableOverridesProjectDir)
{
    SetEnv("ATELIER_GAME_PROJECT", projectDir_.string().c_str());
    const auto found = GameProject::FindDefaultGameProjectDir();
    UnsetEnv("ATELIER_GAME_PROJECT");

    EXPECT_EQ(found, projectDir_);
}

TEST_F(GameProjectTest, NonexistentEnvironmentPathIsIgnored)
{
    const auto missing = projectDir_ / "does_not_exist";
    SetEnv("ATELIER_GAME_PROJECT", missing.string().c_str());
    const auto found = GameProject::FindDefaultGameProjectDir();
    UnsetEnv("ATELIER_GAME_PROJECT");

    EXPECT_NE(found, missing);
}
