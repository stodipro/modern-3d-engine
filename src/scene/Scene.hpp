#pragma once
#include <vector>
#include <memory>
#include "Entity.hpp"

class Scene {
public:
    Scene() = default;
    ~Scene() = default;

    void update(float deltaTime);
    void render();

    Entity* createEntity(const std::string& name = "Entity");
    void destroyEntity(Entity* entity);
    
    const std::vector<std::unique_ptr<Entity>>& getEntities() const { return entities; }

private:
    std::vector<std::unique_ptr<Entity>> entities;
};
