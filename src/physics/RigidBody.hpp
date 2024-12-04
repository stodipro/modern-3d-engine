#pragma once
#include <glm/glm.hpp>
#include "../components/Component.hpp"

class RigidBody : public Component {
public:
    RigidBody();
    ~RigidBody();

    void update(float deltaTime) override;

    // Properties
    void setMass(float mass);
    void setDrag(float drag);
    void setAngularDrag(float drag);
    void setUseGravity(bool use);
    void setKinematic(bool kinematic);
    void setFreezeRotation(bool freeze);

    // Forces
    void addForce(const glm::vec3& force);
    void addTorque(const glm::vec3& torque);
    void addImpulse(const glm::vec3& impulse);
    void addAngularImpulse(const glm::vec3& impulse);

    // Getters
    float getMass() const { return mass; }
    bool isKinematic() const { return isKinematic; }
    const glm::vec3& getVelocity() const { return velocity; }
    const glm::vec3& getAngularVelocity() const { return angularVelocity; }

    // Physics state
    void setVelocity(const glm::vec3& vel);
    void setAngularVelocity(const glm::vec3& angVel);

private:
    float mass;
    float drag;
    float angularDrag;
    bool useGravity;
    bool kinematic;
    bool freezeRotation;

    glm::vec3 velocity;
    glm::vec3 angularVelocity;
    glm::vec3 forces;
    glm::vec3 torques;

    friend class PhysicsSystem;
};
