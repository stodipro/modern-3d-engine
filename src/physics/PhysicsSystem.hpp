#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>

class RigidBody;
class Collider;

class PhysicsSystem {
public:
    static PhysicsSystem& getInstance() {
        static PhysicsSystem instance;
        return instance;
    }

    void initialize();
    void update(float deltaTime);

    // Configuration
    void setGravity(const glm::vec3& gravity) { this->gravity = gravity; }
    void setIterations(int iterations) { this->solverIterations = iterations; }

    // Object management
    void addRigidBody(RigidBody* body);
    void removeRigidBody(RigidBody* body);
    void addCollider(Collider* collider);
    void removeCollider(Collider* collider);

    // Raycasting
    struct RaycastHit {
        Collider* collider;
        glm::vec3 point;
        glm::vec3 normal;
        float distance;
    };

    bool raycast(const glm::vec3& origin, const glm::vec3& direction, RaycastHit& hit, float maxDistance = 1000.0f);

private:
    PhysicsSystem() = default;
    ~PhysicsSystem() = default;
    PhysicsSystem(const PhysicsSystem&) = delete;
    PhysicsSystem& operator=(const PhysicsSystem&) = delete;

    std::vector<RigidBody*> rigidBodies;
    std::vector<Collider*> colliders;

    glm::vec3 gravity;
    int solverIterations;

    void updateRigidbodies(float deltaTime);
    void detectCollisions();
    void resolveCollisions();
};
