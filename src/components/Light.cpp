#include "Light.hpp"
#include "Transform.hpp"

Light::Light(Type type)
    : type(type)
    , color(1.0f)
    , intensity(1.0f)
    , range(10.0f)
    , spotAngle(45.0f)
{}

void Light::update(float deltaTime) {
    // No update logic needed for now
}

glm::vec3 Light::getDirection() const {
    if (auto transform = getEntity()->getComponent<Transform>()) {
        // Use the forward vector of the transform
        glm::mat4 rotation = glm::mat4_cast(transform->getRotation());
        return -glm::normalize(glm::vec3(rotation * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));
    }
    return glm::vec3(0.0f, -1.0f, 0.0f); // Default down direction
}

glm::vec3 Light::getPosition() const {
    if (auto transform = getEntity()->getComponent<Transform>()) {
        return transform->getPosition();
    }
    return glm::vec3(0.0f); // Default at origin
}
