#include "Camera.hpp"
#include "Transform.hpp"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
    : projType(ProjectionType::Perspective)
    , fov(45.0f)
    , aspectRatio(16.0f/9.0f)
    , nearPlane(0.1f)
    , farPlane(1000.0f)
    , orthoLeft(-10.0f)
    , orthoRight(10.0f)
    , orthoBottom(-10.0f)
    , orthoTop(10.0f)
    , projectionMatrix(1.0f)
    , dirtyProjection(true)
{}

void Camera::update(float deltaTime) {
    if (dirtyProjection) {
        updateProjectionMatrix();
    }
}

void Camera::setPerspective(float fovDegrees, float ratio, float near, float far) {
    projType = ProjectionType::Perspective;
    fov = fovDegrees;
    aspectRatio = ratio;
    nearPlane = near;
    farPlane = far;
    dirtyProjection = true;
}

void Camera::setOrthographic(float left, float right, float bottom, float top, float near, float far) {
    projType = ProjectionType::Orthographic;
    orthoLeft = left;
    orthoRight = right;
    orthoBottom = bottom;
    orthoTop = top;
    nearPlane = near;
    farPlane = far;
    dirtyProjection = true;
}

void Camera::setProjectionType(ProjectionType type) {
    projType = type;
    dirtyProjection = true;
}

void Camera::setFOV(float degrees) {
    fov = degrees;
    dirtyProjection = true;
}

void Camera::setAspectRatio(float ratio) {
    aspectRatio = ratio;
    dirtyProjection = true;
}

void Camera::setNearPlane(float near) {
    nearPlane = near;
    dirtyProjection = true;
}

void Camera::setFarPlane(float far) {
    farPlane = far;
    dirtyProjection = true;
}

const glm::mat4& Camera::getProjectionMatrix() const {
    return projectionMatrix;
}

glm::mat4 Camera::getViewMatrix() const {
    if (auto transform = getEntity()->getComponent<Transform>()) {
        // Inverse of world transform is our view matrix
        return glm::inverse(transform->getWorldMatrix());
    }
    return glm::mat4(1.0f);
}

void Camera::updateProjectionMatrix() {
    if (projType == ProjectionType::Perspective) {
        projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    } else {
        projectionMatrix = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, nearPlane, farPlane);
    }
    dirtyProjection = false;
}
