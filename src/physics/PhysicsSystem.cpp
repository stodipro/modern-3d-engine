#include "PhysicsSystem.hpp"
#include "RigidBody.hpp"
#include "Collider.hpp"
#include <algorithm>

struct CollisionPair {
    Collider* colliderA;
    Collider* colliderB;
    glm::vec3 contactPoint;
    glm::vec3 normal;
    float depth;
};

void PhysicsSystem::initialize() {
    gravity = glm::vec3(0.0f, -9.81f, 0.0f);
    solverIterations = 4;
}

void PhysicsSystem::update(float deltaTime) {
    // Update rigidbodies
    updateRigidbodies(deltaTime);

    // Handle collisions
    detectCollisions();
    resolveCollisions();
}

void PhysicsSystem::addRigidBody(RigidBody* body) {
    if (body && std::find(rigidBodies.begin(), rigidBodies.end(), body) == rigidBodies.end()) {
        rigidBodies.push_back(body);
    }
}

void PhysicsSystem::removeRigidBody(RigidBody* body) {
    auto it = std::find(rigidBodies.begin(), rigidBodies.end(), body);
    if (it != rigidBodies.end()) {
        rigidBodies.erase(it);
    }
}

void PhysicsSystem::addCollider(Collider* collider) {
    if (collider && std::find(colliders.begin(), colliders.end(), collider) == colliders.end()) {
        colliders.push_back(collider);
    }
}

void PhysicsSystem::removeCollider(Collider* collider) {
    auto it = std::find(colliders.begin(), colliders.end(), collider);
    if (it != colliders.end()) {
        colliders.erase(it);
    }
}

void PhysicsSystem::updateRigidbodies(float deltaTime) {
    for (auto body : rigidBodies) {
        if (body && !body->isKinematic()) {
            body->update(deltaTime);
        }
    }
}

void PhysicsSystem::detectCollisions() {
    std::vector<CollisionPair> collisions;

    // Broad phase - simple O(n²) for now, could be optimized with spatial partitioning
    for (size_t i = 0; i < colliders.size(); ++i) {
        for (size_t j = i + 1; j < colliders.size(); ++j) {
            Collider* colliderA = colliders[i];
            Collider* colliderB = colliders[j];

            // Skip if either collider is null
            if (!colliderA || !colliderB) continue;

            // Check bounding boxes first
            glm::vec3 minA, maxA, minB, maxB;
            colliderA->calculateBounds(minA, maxA);
            colliderB->calculateBounds(minB, maxB);

            if (maxA.x < minB.x || minA.x > maxB.x ||
                maxA.y < minB.y || minA.y > maxB.y ||
                maxA.z < minB.z || minA.z > maxB.z) {
                continue; // No collision possible
            }

            // Narrow phase - detailed collision check
            CollisionPair pair;
            pair.colliderA = colliderA;
            pair.colliderB = colliderB;

            if (colliderA->checkCollision(colliderB, pair.contactPoint, pair.normal, pair.depth)) {
                collisions.push_back(pair);
            }
        }
    }

    // Resolve collisions
    for (const auto& collision : collisions) {
        // Get rigidbodies if they exist
        auto entityA = collision.colliderA->getEntity();
        auto entityB = collision.colliderB->getEntity();
        auto rbA = entityA ? entityA->getComponent<RigidBody>() : nullptr;
        auto rbB = entityB ? entityB->getComponent<RigidBody>() : nullptr;

        // Handle collision response
        if (rbA || rbB) {
            // Calculate relative velocity
            glm::vec3 relativeVel = glm::vec3(0.0f);
            if (rbA) relativeVel += rbA->getVelocity();
            if (rbB) relativeVel -= rbB->getVelocity();

            float restitution = std::min(collision.colliderA->getRestitution(),
                                       collision.colliderB->getRestitution());

            // Calculate impulse
            float velAlongNormal = glm::dot(relativeVel, collision.normal);
            if (velAlongNormal > 0) continue; // Objects are separating

            float j = -(1.0f + restitution) * velAlongNormal;
            if (rbA && rbB) {
                j /= 1.0f/rbA->getMass() + 1.0f/rbB->getMass();
            } else {
                j /= rbA ? 1.0f/rbA->getMass() : 1.0f/rbB->getMass();
            }

            glm::vec3 impulse = j * collision.normal;

            // Apply impulse
            if (rbA && !rbA->isKinematic()) {
                rbA->addImpulse(-impulse);
            }
            if (rbB && !rbB->isKinematic()) {
                rbB->addImpulse(impulse);
            }

            // Positional correction to prevent sinking
            const float percent = 0.2f; // penetration percentage to correct
            const float slop = 0.01f; // penetration allowance
            glm::vec3 correction = std::max(collision.depth - slop, 0.0f) / 
                                  (rbA && !rbA->isKinematic() && rbB && !rbB->isKinematic() ? 2.0f : 1.0f) * 
                                  percent * collision.normal;

            if (rbA && !rbA->isKinematic()) {
                auto transformA = entityA->getComponent<Transform>();
                transformA->setPosition(transformA->getPosition() - correction);
            }
            if (rbB && !rbB->isKinematic()) {
                auto transformB = entityB->getComponent<Transform>();
                transformB->setPosition(transformB->getPosition() + correction);
            }
        }
    }
}

void PhysicsSystem::resolveCollisions() {
    // Additional collision response and constraint solving could be added here
}

bool PhysicsSystem::raycast(const glm::vec3& origin, const glm::vec3& direction, RaycastHit& hit, float maxDistance) {
    // Simple raycast implementation
    glm::vec3 normalizedDir = glm::normalize(direction);
    float closestHit = maxDistance;
    bool foundHit = false;

    for (auto collider : colliders) {
        if (!collider) continue;

        // Simple sphere test for now
        if (auto sphere = dynamic_cast<SphereCollider*>(collider)) {
            auto transform = collider->getEntity()->getComponent<Transform>();
            glm::vec3 center = transform->getPosition();
            float radius = sphere->getRadius();

            glm::vec3 oc = origin - center;
            float a = glm::dot(normalizedDir, normalizedDir);
            float b = 2.0f * glm::dot(oc, normalizedDir);
            float c = glm::dot(oc, oc) - radius * radius;
            float discriminant = b * b - 4 * a * c;

            if (discriminant > 0) {
                float t = (-b - sqrt(discriminant)) / (2.0f * a);
                if (t > 0 && t < closestHit) {
                    closestHit = t;
                    hit.collider = collider;
                    hit.point = origin + normalizedDir * t;
                    hit.normal = glm::normalize(hit.point - center);
                    hit.distance = t;
                    foundHit = true;
                }
            }
        }
    }

    return foundHit;
}
