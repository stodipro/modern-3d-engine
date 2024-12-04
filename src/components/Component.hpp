#pragma once

class Entity;

class Component {
public:
    Component() : entity(nullptr) {}
    virtual ~Component() = default;

    virtual void update(float deltaTime) {}
    virtual void render() {}

    Entity* getEntity() { return entity; }

protected:
    Entity* entity;

    friend class Entity;
};
