#include "Entity.hpp"

Entity::Entity(const std::string& entityName) 
    : name(entityName) {}

void Entity::update(float deltaTime) {
    for (auto& component : components) {
        component->update(deltaTime);
    }
}
