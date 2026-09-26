#pragma once

#include <cstdint>

namespace Engine::Scene
{
    class Entity
    {
    public:
        using Id = std::uint32_t;

        explicit Entity(Id id = 0) : id_(id) {}

        Id GetId() const { return id_; }
        explicit operator bool() const { return id_ != 0; }

    private:
        Id id_ = 0;
    };
}
