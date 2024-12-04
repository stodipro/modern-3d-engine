#include "FPSController.hpp"
#include "../core/Input.hpp"
#include "../components/Transform.hpp"
#include "../components/Camera.hpp"
#include "../physics/RigidBody.hpp"
#include "../physics/CapsuleCollider.hpp"
#include <GLFW/glfw3.h>

FPSController::FPSController()
    : moveSpeed(5.0f)
    , sprintMultiplier(1.5f)
    , jumpForce(5.0f)
    , mouseSensitivity(0.2f)
    , pitch(0.0f)
    , yaw(0.0f)
    , maxPitchAngle(89.0f)
    , grounded(false)
    , sprinting(false)
    , crouching(false)
    , crouchHeight(1.0f)
    , standingHeight(2.0f)
    , velocity(0.0f)
    , moveDirection(0.0f)
{}

void FPSController::update(float deltaTime) {
    handleMovementInput(deltaTime);
    handleMouseInput(deltaTime);
    updatePhysics(deltaTime);
}

void FPSController::handleMovementInput(float deltaTime) {
    auto& input = Input::getInstance();
    auto transform = getEntity()->getComponent<Transform>();
    
    // Get movement input
    glm::vec3 inputDir(0.0f);
    if (input.isKeyPressed(GLFW_KEY_W)) inputDir.z += 1.0f;
    if (input.isKeyPressed(GLFW_KEY_S)) inputDir.z -= 1.0f;
    if (input.isKeyPressed(GLFW_KEY_A)) inputDir.x -= 1.0f;
    if (input.isKeyPressed(GLFW_KEY_D)) inputDir.x += 1.0f;

    // Normalize input direction
    if (glm::length(inputDir) > 0.0f) {
        inputDir = glm::normalize(inputDir);
    }

    // Convert input to world space
    glm::vec3 forward = transform->getForward();
    forward.y = 0.0f;
    forward = glm::normalize(forward);
    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));

    moveDirection = forward * inputDir.z + right * inputDir.x;

    // Handle sprint
    if (input.isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
        startSprint();
    } else {
        stopSprint();
    }

    // Handle crouch
    if (input.isKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
        startCrouch();
    } else {
        stopCrouch();
    }

    // Handle jump
    if (input.isKeyPressed(GLFW_KEY_SPACE) && grounded) {
        jump();
    }
}

void FPSController::handleMouseInput(float deltaTime) {
    auto& input = Input::getInstance();
    auto transform = getEntity()->getComponent<Transform>();

    // Get mouse movement
    glm::vec2 mouseDelta = input.getMouseDelta() * mouseSensitivity;

    // Update camera angles
    yaw += mouseDelta.x;
    pitch = glm::clamp(pitch - mouseDelta.y, -maxPitchAngle, maxPitchAngle);

    // Update camera rotation
    transform->setRotation(glm::quat(
        glm::vec3(glm::radians(pitch), glm::radians(yaw), 0.0f)
    ));
}

void FPSController::updatePhysics(float deltaTime) {
    auto rb = getEntity()->getComponent<RigidBody>();
    if (!rb) return;

    // Check if grounded
    checkGrounded();

    // Apply movement
    float currentSpeed = moveSpeed * (sprinting ? sprintMultiplier : 1.0f);
    glm::vec3 targetVelocity = moveDirection * currentSpeed;
    targetVelocity.y = rb->getVelocity().y;

    // Smooth velocity change
    velocity = glm::mix(velocity, targetVelocity, deltaTime * 10.0f);
    rb->setVelocity(velocity);

    // Update collider height based on crouch state
    if (auto capsule = getEntity()->getComponent<CapsuleCollider>()) {
        float targetHeight = crouching ? crouchHeight : standingHeight;
        float currentHeight = capsule->getHeight();
        capsule->setHeight(glm::mix(currentHeight, targetHeight, deltaTime * 10.0f));
    }
}

void FPSController::checkGrounded() {
    auto transform = getEntity()->getComponent<Transform>();
    auto capsule = getEntity()->getComponent<CapsuleCollider>();

    if (!transform || !capsule) return;

    // Cast ray downward to check for ground
    PhysicsSystem::RaycastHit hit;
    glm::vec3 start = transform->getPosition();
    glm::vec3 end = start - glm::vec3(0.0f, 0.1f, 0.0f);

    grounded = PhysicsSystem::getInstance().raycast(start, end - start, hit);
}

void FPSController::jump() {
    if (!grounded) return;

    auto rb = getEntity()->getComponent<RigidBody>();
    if (rb) {
        velocity.y = jumpForce;
        rb->setVelocity(velocity);
        grounded = false;
    }
}

void FPSController::startSprint() {
    if (!crouching) {
        sprinting = true;
    }
}

void FPSController::stopSprint() {
    sprinting = false;
}

void FPSController::startCrouch() {
    crouching = true;
    sprinting = false;
}

void FPSController::stopCrouch() {
    // Check if we can stand up
    auto transform = getEntity()->getComponent<Transform>();
    if (transform) {
        PhysicsSystem::RaycastHit hit;
        glm::vec3 start = transform->getPosition();
        glm::vec3 end = start + glm::vec3(0.0f, standingHeight - crouchHeight, 0.0f);

        if (!PhysicsSystem::getInstance().raycast(start, end - start, hit)) {
            crouching = false;
        }
    }
}
