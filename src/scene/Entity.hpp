#pragma once
#include <string>
#include <vector>
#include <memory>
#include <typeinfo>
#include "../components/Component.hpp"

class Entity {
public:
    Entity(const std::string& name = "Entity");
    ~Entity() = default;

    void update(float deltaTime);

    template<typename T, typename... Args>
    T* addComponent(Args&&... args) {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T* componentPtr = component.get();
        component->entity = this;
        components.push_back(std::move(component));
        return componentPtr;
    }

    template<typename T>
    T* getComponent() {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
        for (const auto& component : components) {
            if (T* casted = dynamic_cast<T*>(component.get())) {
                return casted;
            }
        }
        return nullptr;
    }

    const std::string& getName() const { return name; }
    void setName(const std::string& newName) { name = newName; }

private:
    std::string name;
    std::vector<std::unique_ptr<Component>> components;
};
