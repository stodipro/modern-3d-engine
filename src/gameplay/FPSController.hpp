#pragma once
#include "../components/Component.hpp"
#include <glm/glm.hpp>

class FPSController : public Component {
public:
    FPSController();
    void update(float deltaTime) override;

    // Movement settings
    void setMoveSpeed(float speed) { moveSpeed = speed; }
    void setSprintMultiplier(float multiplier) { sprintMultiplier = multiplier; }
    void setJumpForce(float force) { jumpForce = force; }
    void setMouseSensitivity(float sensitivity) { mouseSensitivity = sensitivity; }

    // State getters
    bool isGrounded() const { return grounded; }
    bool isSprinting() const { return sprinting; }
    bool isCrouching() const { return crouching; }

    // Movement controls
    void jump();
    void startSprint();
    void stopSprint();
    void startCrouch();
    void stopCrouch();

private:
    // Movement parameters
    float moveSpeed;
    float sprintMultiplier;
    float jumpForce;
    float mouseSensitivity;

    // Camera parameters
    float pitch;
    float yaw;
    float maxPitchAngle;

    // State
    bool grounded;
    bool sprinting;
    bool crouching;
    float crouchHeight;
    float standingHeight;

    // Movement state
    glm::vec3 velocity;
    glm::vec3 moveDirection;

    // Internal methods
    void handleMovementInput(float deltaTime);
    void handleMouseInput(float deltaTime);
    void updatePhysics(float deltaTime);
    void checkGrounded();
};
