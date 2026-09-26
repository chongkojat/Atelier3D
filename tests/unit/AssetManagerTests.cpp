#include "engine/assets/AssetManager.hpp"

#include <gtest/gtest.h>

using Engine::Assets::AssetManager;

TEST(AssetManager, DefaultSearchPathIsCurrentDirectory)
{
    const AssetManager assets;
    EXPECT_EQ(assets.GetSearchPath(), ".");
}

TEST(AssetManager, SetSearchPath)
{
    AssetManager assets;
    assets.SetSearchPath("Assets");
    EXPECT_EQ(assets.GetSearchPath(), "Assets");
}

TEST(AssetManager, ResolvesRegisteredVirtualPath)
{
    AssetManager assets;
    assets.SetSearchPath("Assets");
    assets.RegisterVirtualPath("cube", "Models/cube.obj");

    EXPECT_EQ(assets.Resolve("cube"), "Assets/Models/cube.obj");
}

TEST(AssetManager, UnregisteredNameIsReturnedUnchanged)
{
    const AssetManager assets;
    EXPECT_EQ(assets.Resolve("Textures/wood.png"), "Textures/wood.png");
}

TEST(AssetManager, ReRegisteringOverridesPreviousPath)
{
    AssetManager assets;
    assets.RegisterVirtualPath("cube", "old.obj");
    assets.RegisterVirtualPath("cube", "new.obj");

    EXPECT_EQ(assets.Resolve("cube"), "./new.obj");
}
