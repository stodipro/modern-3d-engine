#include "Scene.hpp"
#include <algorithm>

void Scene::update(float deltaTime) {
    for (auto& entity : entities) {
        entity->update(deltaTime);
    }
}

void Scene::render() {
    for (auto& entity : entities) {
        // Render each entity's components
        for (const auto& component : entity->components) {
            component->render();
        }
    }
}

Entity* Scene::createEntity(const std::string& name) {
    entities.push_back(std::make_unique<Entity>(name));
    return entities.back().get();
}

void Scene::destroyEntity(Entity* entity) {
    auto it = std::find_if(entities.begin(), entities.end(),
        [entity](const std::unique_ptr<Entity>& e) { return e.get() == entity; });
    
    if (it != entities.end()) {
        entities.erase(it);
    }
}
