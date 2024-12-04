#include "Collider.hpp"
#include "../components/Transform.hpp"

Collider::Collider(Type type)
    : type(type)
    , trigger(false)
    , restitution(0.6f)
    , friction(0.5f)
{}

Collider::~Collider() {}

// BoxCollider implementation
BoxCollider::BoxCollider()
    : Collider(Type::Box)
    , size(1.0f)
{}

bool BoxCollider::checkCollision(const Collider* other, glm::vec3& contactPoint, glm::vec3& normal, float& depth) const {
    if (auto otherBox = dynamic_cast<const BoxCollider*>(other)) {
        // Box-Box collision
        auto transform1 = getEntity()->getComponent<Transform>();
        auto transform2 = other->getEntity()->getComponent<Transform>();

        glm::vec3 pos1 = transform1->getPosition();
        glm::vec3 pos2 = transform2->getPosition();

        // Simple AABB check for now
        glm::vec3 min1 = pos1 - size * 0.5f;
        glm::vec3 max1 = pos1 + size * 0.5f;
        glm::vec3 min2 = pos2 - otherBox->size * 0.5f;
        glm::vec3 max2 = pos2 + otherBox->size * 0.5f;

        if (min1.x <= max2.x && max1.x >= min2.x &&
            min1.y <= max2.y && max1.y >= min2.y &&
            min1.z <= max2.z && max1.z >= min2.z) {
            
            // Find the contact point and normal
            glm::vec3 center1 = (min1 + max1) * 0.5f;
            glm::vec3 center2 = (min2 + max2) * 0.5f;
            glm::vec3 delta = center2 - center1;

            // Find the largest overlap
            float xOverlap = (max1.x - min1.x + max2.x - min2.x) * 0.5f - std::abs(delta.x);
            float yOverlap = (max1.y - min1.y + max2.y - min2.y) * 0.5f - std::abs(delta.y);
            float zOverlap = (max1.z - min1.z + max2.z - min2.z) * 0.5f - std::abs(delta.z);

            // Use smallest overlap for collision resolution
            if (xOverlap < yOverlap && xOverlap < zOverlap) {
                depth = xOverlap;
                normal = glm::vec3(delta.x < 0 ? -1.0f : 1.0f, 0.0f, 0.0f);
            } else if (yOverlap < zOverlap) {
                depth = yOverlap;
                normal = glm::vec3(0.0f, delta.y < 0 ? -1.0f : 1.0f, 0.0f);
            } else {
                depth = zOverlap;
                normal = glm::vec3(0.0f, 0.0f, delta.z < 0 ? -1.0f : 1.0f);
            }

            contactPoint = center1 + normal * depth * 0.5f;
            return true;
        }
    }
    else if (auto sphere = dynamic_cast<const SphereCollider*>(other)) {
        // Box-Sphere collision
        // TODO: Implement Box-Sphere collision
    }

    return false;
}

void BoxCollider::calculateBounds(glm::vec3& min, glm::vec3& max) const {
    auto transform = getEntity()->getComponent<Transform>();
    glm::vec3 pos = transform->getPosition();
    min = pos - size * 0.5f;
    max = pos + size * 0.5f;
}

// SphereCollider implementation
SphereCollider::SphereCollider()
    : Collider(Type::Sphere)
    , radius(0.5f)
{}

bool SphereCollider::checkCollision(const Collider* other, glm::vec3& contactPoint, glm::vec3& normal, float& depth) const {
    if (auto otherSphere = dynamic_cast<const SphereCollider*>(other)) {
        // Sphere-Sphere collision
        auto transform1 = getEntity()->getComponent<Transform>();
        auto transform2 = other->getEntity()->getComponent<Transform>();

        glm::vec3 pos1 = transform1->getPosition();
        glm::vec3 pos2 = transform2->getPosition();

        float radiusSum = radius + otherSphere->radius;
        glm::vec3 delta = pos2 - pos1;
        float distSquared = glm::dot(delta, delta);

        if (distSquared <= radiusSum * radiusSum) {
            float dist = std::sqrt(distSquared);
            normal = dist > 0 ? delta / dist : glm::vec3(0, 1, 0);
            depth = radiusSum - dist;
            contactPoint = pos1 + normal * (radius - depth * 0.5f);
            return true;
        }
    }
    else if (auto box = dynamic_cast<const BoxCollider*>(other)) {
        // Sphere-Box collision
        // TODO: Implement Sphere-Box collision
    }

    return false;
}

void SphereCollider::calculateBounds(glm::vec3& min, glm::vec3& max) const {
    auto transform = getEntity()->getComponent<Transform>();
    glm::vec3 pos = transform->getPosition();
    glm::vec3 extent(radius);
    min = pos - extent;
    max = pos + extent;
}
