#include "engine/input/Input.hpp"

namespace Engine::Input
{
    void InputSystem::SetKey(int keyCode, KeyState state)
    {
        if (keyCode < 0 || keyCode >= MaxKeys)
        {
            return;
        }
        keys_[keyCode] = state;
    }

    KeyState InputSystem::GetKey(int keyCode) const
    {
        if (keyCode < 0 || keyCode >= MaxKeys)
        {
            return KeyState::Up;
        }
        return keys_[keyCode];
    }
}
