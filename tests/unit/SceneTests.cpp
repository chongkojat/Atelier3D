#include "engine/scene/Scene.hpp"

#include <gtest/gtest.h>

using Engine::Scene::Entity;
using Engine::Scene::Scene;

TEST(Entity, DefaultEntityIsInvalid)
{
    const Entity entity;
    EXPECT_FALSE(entity);
    EXPECT_EQ(entity.GetId(), 0u);
}

TEST(Scene, StartsEmpty)
{
    const Scene scene;
    EXPECT_TRUE(scene.Entities().empty());
}

TEST(Scene, CreateEntityAssignsIncreasingIds)
{
    Scene scene;
    const Entity a = scene.CreateEntity();
    const Entity b = scene.CreateEntity();

    EXPECT_TRUE(a);
    EXPECT_TRUE(b);
    EXPECT_EQ(a.GetId(), 1u);
    EXPECT_EQ(b.GetId(), 2u);
    EXPECT_EQ(scene.Entities().size(), 2u);
}

TEST(Scene, DestroyEntityRemovesOnlyThatEntity)
{
    Scene scene;
    const Entity a = scene.CreateEntity();
    const Entity b = scene.CreateEntity();
    const Entity c = scene.CreateEntity();

    scene.DestroyEntity(b);

    ASSERT_EQ(scene.Entities().size(), 2u);
    EXPECT_EQ(scene.Entities()[0].GetId(), a.GetId());
    EXPECT_EQ(scene.Entities()[1].GetId(), c.GetId());
}

TEST(Scene, DestroyUnknownEntityDoesNothing)
{
    Scene scene;
    scene.CreateEntity();

    scene.DestroyEntity(Entity{42});

    EXPECT_EQ(scene.Entities().size(), 1u);
}

TEST(Scene, IdsAreNotReusedAfterDestroy)
{
    Scene scene;
    const Entity a = scene.CreateEntity();
    scene.DestroyEntity(a);

    const Entity b = scene.CreateEntity();
    EXPECT_NE(a.GetId(), b.GetId());
}
