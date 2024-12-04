#pragma once
#include "Collider.hpp"

class CapsuleCollider : public Collider {
public:
    CapsuleCollider();

    // Capsule properties
    void setRadius(float radius) { this->radius = radius; }
    void setHeight(float height) { this->height = height; }
    
    float getRadius() const { return radius; }
    float getHeight() const { return height; }

    // Line segment representing capsule's central axis
    void getSegment(glm::vec3& start, glm::vec3& end) const;

    // Inherited from Collider
    bool checkCollision(const Collider* other, glm::vec3& contactPoint, glm::vec3& normal, float& depth) const override;
    void calculateBounds(glm::vec3& min, glm::vec3& max) const override;

private:
    float radius;
    float height; // Height of the cylindrical portion

    // Helper functions for collision detection
    bool capsuleCapsule(const CapsuleCollider* other, glm::vec3& contactPoint, glm::vec3& normal, float& depth) const;
    bool capsuleSphere(const SphereCollider* sphere, glm::vec3& contactPoint, glm::vec3& normal, float& depth) const;
    bool capsuleBox(const BoxCollider* box, glm::vec3& contactPoint, glm::vec3& normal, float& depth) const;
};
