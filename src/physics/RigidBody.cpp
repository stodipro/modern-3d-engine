#include "RigidBody.hpp"
#include "PhysicsSystem.hpp"
#include "../components/Transform.hpp"

RigidBody::RigidBody()
    : mass(1.0f)
    , drag(0.01f)
    , angularDrag(0.05f)
    , useGravity(true)
    , kinematic(false)
    , freezeRotation(false)
    , velocity(0.0f)
    , angularVelocity(0.0f)
    , forces(0.0f)
    , torques(0.0f)
{
    PhysicsSystem::getInstance().addRigidBody(this);
}

RigidBody::~RigidBody() {
    PhysicsSystem::getInstance().removeRigidBody(this);
}

void RigidBody::update(float deltaTime) {
    if (kinematic) return;

    auto transform = getEntity()->getComponent<Transform>();
    if (!transform) return;

    // Apply forces
    glm::vec3 acceleration = forces / mass;
    if (useGravity) {
        acceleration += PhysicsSystem::getInstance().getGravity();
    }

    // Update linear velocity
    velocity += acceleration * deltaTime;
    velocity *= (1.0f - drag);

    // Update position
    transform->setPosition(transform->getPosition() + velocity * deltaTime);

    // Handle rotation if not frozen
    if (!freezeRotation) {
        // Update angular velocity
        angularVelocity += torques * deltaTime;
        angularVelocity *= (1.0f - angularDrag);

        // Update rotation
        glm::quat rotation = transform->getRotation();
        glm::quat deltaRotation = glm::quat(0.0f, angularVelocity * deltaTime);
        transform->setRotation(rotation * deltaRotation);
    }

    // Clear forces and torques
    forces = glm::vec3(0.0f);
    torques = glm::vec3(0.0f);
}

void RigidBody::setMass(float newMass) {
    mass = glm::max(newMass, 0.0001f);
}

void RigidBody::setDrag(float newDrag) {
    drag = glm::clamp(newDrag, 0.0f, 1.0f);
}

void RigidBody::setAngularDrag(float newDrag) {
    angularDrag = glm::clamp(newDrag, 0.0f, 1.0f);
}

void RigidBody::setUseGravity(bool use) {
    useGravity = use;
}

void RigidBody::setKinematic(bool newKinematic) {
    kinematic = newKinematic;
    if (kinematic) {
        velocity = glm::vec3(0.0f);
        angularVelocity = glm::vec3(0.0f);
    }
}

void RigidBody::setFreezeRotation(bool freeze) {
    freezeRotation = freeze;
    if (freezeRotation) {
        angularVelocity = glm::vec3(0.0f);
    }
}

void RigidBody::addForce(const glm::vec3& force) {
    forces += force;
}

void RigidBody::addTorque(const glm::vec3& torque) {
    if (!freezeRotation) {
        torques += torque;
    }
}

void RigidBody::addImpulse(const glm::vec3& impulse) {
    if (!kinematic) {
        velocity += impulse / mass;
    }
}

void RigidBody::addAngularImpulse(const glm::vec3& impulse) {
    if (!kinematic && !freezeRotation) {
        angularVelocity += impulse;
    }
}

void RigidBody::setVelocity(const glm::vec3& vel) {
    velocity = vel;
}

void RigidBody::setAngularVelocity(const glm::vec3& angVel) {
    if (!freezeRotation) {
        angularVelocity = angVel;
    }
}
