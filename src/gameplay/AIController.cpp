#include "AIController.hpp"
#include "../components/Transform.hpp"
#include "../physics/PhysicsSystem.hpp"
#include "WeaponSystem.hpp"
#include "HealthSystem.hpp"
#include <algorithm>

bool AIController::checkTargetVisibility() {
    if (!currentTarget) return false;

    auto transform = getEntity()->getComponent<Transform>();
    auto targetTransform = currentTarget->getComponent<Transform>();

    if (!transform || !targetTransform) return false;

    glm::vec3 toTarget = targetTransform->getPosition() - transform->getPosition();
    float distance = glm::length(toTarget);

    if (distance > aggressionRange) return false;

    // Raycast to check for obstacles
    PhysicsSystem::RaycastHit hit;
    if (PhysicsSystem::getInstance().raycast(transform->getPosition(), 
                                           glm::normalize(toTarget), 
                                           hit, 
                                           distance)) {
        // Check if we hit the target
        return hit.collider->getEntity() == currentTarget;
    }

    return false;
}

void AIController::updateIdleState(float deltaTime) {
    // Periodically look around
    if (stateTimer > 3.0f) {
        setState(State::Patrol);
    }
}

void AIController::updatePatrolState(float deltaTime) {
    if (patrolPoints.empty()) return;

    auto transform = getEntity()->getComponent<Transform>();
    if (!transform) return;

    // Move to next patrol point
    glm::vec3 targetPos = patrolPoints[currentPatrolPoint];
    moveToPosition(targetPos, deltaTime);

    // Check if we reached the current patrol point
    float distToPoint = glm::length(targetPos - transform->getPosition());
    if (distToPoint < 0.5f) {
        // Move to next point
        currentPatrolPoint = (currentPatrolPoint + 1) % patrolPoints.size();
        stateTimer = 0.0f;
    }
}

void AIController::updateChaseState(float deltaTime) {
    if (!currentTarget) {
        setState(State::Patrol);
        return;
    }

    auto targetTransform = currentTarget->getComponent<Transform>();
    if (!targetTransform) return;

    // Move towards target while maintaining some distance
    glm::vec3 targetPos = targetTransform->getPosition();
    float distanceToTarget = glm::length(targetPos - getEntity()->getComponent<Transform>()->getPosition());

    if (distanceToTarget > attackRange) {
        // Move closer
        moveToPosition(targetPos, deltaTime);
    } else if (distanceToTarget < attackRange * 0.5f) {
        // Move away
        glm::vec3 awayDir = glm::normalize(getEntity()->getComponent<Transform>()->getPosition() - targetPos);
        moveToPosition(getEntity()->getComponent<Transform>()->getPosition() + awayDir * 2.0f, deltaTime);
    }

    // Always face the target
    rotateTowards(targetPos, deltaTime);
}

void AIController::updateAttackState(float deltaTime) {
    if (!currentTarget) {
        setState(State::Patrol);
        return;
    }

    auto targetTransform = currentTarget->getComponent<Transform>();
    if (!targetTransform) return;

    // Face target
    rotateTowards(targetTransform->getPosition(), deltaTime);

    // Check if we should attack
    if (attackCooldown <= 0.0f && canSeeTarget) {
        performAttack();
    }

    // Check if we should seek cover
    auto health = getEntity()->getComponent<HealthSystem>();
    if (health && health->getHealth() < health->getMaxHealth() * 0.3f) {
        setState(State::Cover);
    }
}

void AIController::updateCoverState(float deltaTime) {
    static float coverTimer = 0.0f;
    coverTimer += deltaTime;

    // Find cover position if we haven't yet
    static glm::vec3 coverPosition;
    if (stateTimer < 0.1f) {
        if (!findCoverPosition(coverPosition)) {
            setState(State::Flee);
            return;
        }
    }

    // Move to cover
    moveToPosition(coverPosition, deltaTime);

    // Check if we reached cover
    float distToCover = glm::length(coverPosition - getEntity()->getComponent<Transform>()->getPosition());
    if (distToCover < 0.5f) {
        // Occasionally peek and shoot
        if (coverTimer > 2.0f && canSeeTarget && attackCooldown <= 0.0f) {
            performAttack();
            coverTimer = 0.0f;
        }

        // Heal up
        auto health = getEntity()->getComponent<HealthSystem>();
        if (health && health->getHealth() > health->getMaxHealth() * 0.7f) {
            setState(State::Chase);
        }
    }
}

void AIController::updateFleeState(float deltaTime) {
    if (!currentTarget) {
        setState(State::Patrol);
        return;
    }

    // Run away from target
    auto transform = getEntity()->getComponent<Transform>();
    auto targetTransform = currentTarget->getComponent<Transform>();

    if (!transform || !targetTransform) return;

    glm::vec3 awayDir = glm::normalize(transform->getPosition() - targetTransform->getPosition());
    moveToPosition(transform->getPosition() + awayDir * 5.0f, deltaTime);

    // Return to patrol if we're far enough away
    float distToTarget = glm::length(transform->getPosition() - targetTransform->getPosition());
    if (distToTarget > aggressionRange * 1.5f) {
        setState(State::Patrol);
    }
}

void AIController::moveToPosition(const glm::vec3& position, float deltaTime) {
    auto transform = getEntity()->getComponent<Transform>();
    auto rb = getEntity()->getComponent<RigidBody>();

    if (!transform || !rb) return;

    glm::vec3 direction = position - transform->getPosition();
    direction.y = 0; // Keep movement on the horizontal plane
    
    if (glm::length(direction) > 0.1f) {
        direction = glm::normalize(direction);
        rb->setVelocity(direction * moveSpeed);
        rotateTowards(position, deltaTime);
    } else {
        rb->setVelocity(glm::vec3(0.0f));
    }
}

void AIController::rotateTowards(const glm::vec3& target, float deltaTime) {
    auto transform = getEntity()->getComponent<Transform>();
    if (!transform) return;

    glm::vec3 direction = target - transform->getPosition();
    direction.y = 0;

    if (glm::length(direction) > 0.1f) {
        glm::vec3 forward = transform->getForward();
        forward.y = 0;

        float angle = glm::degrees(acos(glm::dot(glm::normalize(forward), glm::normalize(direction))));
        
        if (angle > 1.0f) {
            float rotationAmount = rotationSpeed * deltaTime;
            if (rotationAmount > angle) rotationAmount = angle;

            // Determine rotation direction
            float cross = glm::cross(forward, direction).y;
            transform->rotate(glm::vec3(0.0f, cross > 0 ? rotationAmount : -rotationAmount, 0.0f));
        }
    }
}

void AIController::performAttack() {
    auto weapons = getEntity()->getComponent<WeaponSystem>();
    if (!weapons) return;

    // Add some inaccuracy
    float hitChance = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    if (hitChance <= accuracy) {
        weapons->startFiring();
    }

    attackCooldown = 1.0f / weapons->getActiveWeapon().fireRate;
}

bool AIController::findCoverPosition(glm::vec3& coverPos) {
    if (!currentTarget) return false;

    auto transform = getEntity()->getComponent<Transform>();
    auto targetTransform = currentTarget->getComponent<Transform>();

    if (!transform || !targetTransform) return false;

    // Sample points in a radius around the AI
    const int numSamples = 16;
    const float radius = 5.0f;

    for (int i = 0; i < numSamples; i++) {
        float angle = (i / float(numSamples)) * 2.0f * 3.14159f;
        glm::vec3 samplePoint = transform->getPosition() + 
                               glm::vec3(cos(angle) * radius, 0.0f, sin(angle) * radius);

        // Check if this point is behind cover relative to the target
        PhysicsSystem::RaycastHit hit;
        glm::vec3 toTarget = targetTransform->getPosition() - samplePoint;
        if (PhysicsSystem::getInstance().raycast(samplePoint, glm::normalize(toTarget), hit, 2.0f)) {
            // Point is behind cover
            coverPos = samplePoint;
            return true;
        }
    }

    return false;
}

void AIController::setState(State newState) {
    if (currentState != newState) {
        currentState = newState;
        stateTimer = 0.0f;
    }
}
