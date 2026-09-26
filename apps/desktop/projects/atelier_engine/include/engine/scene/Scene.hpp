#pragma once

#include "engine/scene/Entity.hpp"

#include <vector>

namespace Engine::Scene
{
    class Scene
    {
    public:
        Entity CreateEntity();
        void DestroyEntity(Entity entity);

        const std::vector<Entity>& Entities() const;

    private:
        std::vector<Entity> entities_;
        Entity::Id nextId_ = 1;
    };
}
