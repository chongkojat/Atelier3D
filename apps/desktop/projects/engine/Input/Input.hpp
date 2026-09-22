#pragma once

#include <cstdint>

namespace Engine::Input
{
    enum class KeyState : std::uint8_t
    {
        Up,
        Down,
    };

    class InputSystem
    {
    public:
        void SetKey(int keyCode, KeyState state);
        KeyState GetKey(int keyCode) const;

    private:
        static constexpr int MaxKeys = 512;
        KeyState keys_[MaxKeys]{};
    };
}
