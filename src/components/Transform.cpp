#include "Transform.hpp"
#include <glm/gtx/matrix_decompose.hpp>

Transform::Transform()
    : position(0.0f)
    , rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f))
    , scale(1.0f)
    , parent(nullptr)
    , dirtyMatrix(true)
    , cachedWorldMatrix(1.0f)
{}

void Transform::update(float deltaTime) {
    if (dirtyMatrix) {
        updateWorldMatrix();
    }
}

void Transform::setPosition(const glm::vec3& pos) {
    position = pos;
    dirtyMatrix = true;
}

const glm::vec3& Transform::getPosition() const {
    return position;
}

void Transform::translate(const glm::vec3& delta) {
    position += delta;
    dirtyMatrix = true;
}

void Transform::setRotation(const glm::quat& rot) {
    rotation = rot;
    dirtyMatrix = true;
}

void Transform::setRotationEuler(const glm::vec3& euler) {
    rotation = glm::quat(glm::radians(euler));
    dirtyMatrix = true;
}

const glm::quat& Transform::getRotation() const {
    return rotation;
}

glm::vec3 Transform::getRotationEuler() const {
    return glm::degrees(glm::eulerAngles(rotation));
}

void Transform::rotate(const glm::vec3& euler) {
    rotation = glm::quat(glm::radians(euler)) * rotation;
    dirtyMatrix = true;
}

void Transform::setScale(const glm::vec3& s) {
    scale = s;
    dirtyMatrix = true;
}

const glm::vec3& Transform::getScale() const {
    return scale;
}

glm::mat4 Transform::getLocalMatrix() const {
    return glm::translate(glm::mat4(1.0f), position) *
           glm::mat4_cast(rotation) *
           glm::scale(glm::mat4(1.0f), scale);
}

glm::mat4 Transform::getWorldMatrix() const {
    return cachedWorldMatrix;
}

void Transform::setParent(Transform* newParent) {
    parent = newParent;
    dirtyMatrix = true;
}

Transform* Transform::getParent() const {
    return parent;
}

void Transform::updateWorldMatrix() {
    if (parent) {
        cachedWorldMatrix = parent->getWorldMatrix() * getLocalMatrix();
    } else {
        cachedWorldMatrix = getLocalMatrix();
    }
    dirtyMatrix = false;
}
