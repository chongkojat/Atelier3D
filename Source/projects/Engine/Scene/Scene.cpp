#include "Engine/Scene/Scene.hpp"

#include <algorithm>

namespace Engine::Scene
{
    Entity Scene::CreateEntity()
    {
        Entity entity{nextId_++};
        entities_.push_back(entity);
        return entity;
    }

    void Scene::DestroyEntity(Entity entity)
    {
        entities_.erase(std::remove_if(entities_.begin(), entities_.end(),
                                       [entity](const Entity& e) { return e.GetId() == entity.GetId(); }),
                        entities_.end());
    }

    const std::vector<Entity>& Scene::Entities() const
    {
        return entities_;
    }
}
