#pragma once
#include <glm/glm.hpp>
#include "../components/Component.hpp"

class Collider : public Component {
public:
    enum class Type {
        Box,
        Sphere,
        Capsule
    };

    Collider(Type type);
    virtual ~Collider();

    // Collision properties
    void setTrigger(bool isTrigger) { trigger = isTrigger; }
    bool isTrigger() const { return trigger; }

    void setMaterial(float restitution, float friction) {
        this->restitution = restitution;
        this->friction = friction;
    }

    // Collision checking
    virtual bool checkCollision(const Collider* other, glm::vec3& contactPoint, glm::vec3& normal, float& depth) const = 0;

    // Bounding box for broad phase
    virtual void calculateBounds(glm::vec3& min, glm::vec3& max) const = 0;

protected:
    Type type;
    bool trigger;
    float restitution;
    float friction;
};

class BoxCollider : public Collider {
public:
    BoxCollider();

    void setSize(const glm::vec3& size) { this->size = size; }
    const glm::vec3& getSize() const { return size; }

    bool checkCollision(const Collider* other, glm::vec3& contactPoint, glm::vec3& normal, float& depth) const override;
    void calculateBounds(glm::vec3& min, glm::vec3& max) const override;

private:
    glm::vec3 size;
};

class SphereCollider : public Collider {
public:
    SphereCollider();

    void setRadius(float radius) { this->radius = radius; }
    float getRadius() const { return radius; }

    bool checkCollision(const Collider* other, glm::vec3& contactPoint, glm::vec3& normal, float& depth) const override;
    void calculateBounds(glm::vec3& min, glm::vec3& max) const override;

private:
    float radius;
};
