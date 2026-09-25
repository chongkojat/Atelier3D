#include "Engine/Input/Input.hpp"

#include <gtest/gtest.h>

using Engine::Input::InputSystem;
using Engine::Input::KeyState;

TEST(InputSystem, KeysStartUp)
{
    const InputSystem input;
    EXPECT_EQ(input.GetKey(0), KeyState::Up);
    EXPECT_EQ(input.GetKey(65), KeyState::Up);
    EXPECT_EQ(input.GetKey(511), KeyState::Up);
}

TEST(InputSystem, SetKeyIsRemembered)
{
    InputSystem input;
    input.SetKey(65, KeyState::Down);
    EXPECT_EQ(input.GetKey(65), KeyState::Down);

    input.SetKey(65, KeyState::Up);
    EXPECT_EQ(input.GetKey(65), KeyState::Up);
}

TEST(InputSystem, SetKeyOnlyAffectsThatKey)
{
    InputSystem input;
    input.SetKey(10, KeyState::Down);
    EXPECT_EQ(input.GetKey(9), KeyState::Up);
    EXPECT_EQ(input.GetKey(11), KeyState::Up);
}

TEST(InputSystem, OutOfRangeKeysAreIgnored)
{
    InputSystem input;
    input.SetKey(-1, KeyState::Down);
    input.SetKey(512, KeyState::Down);

    EXPECT_EQ(input.GetKey(-1), KeyState::Up);
    EXPECT_EQ(input.GetKey(512), KeyState::Up);
}
