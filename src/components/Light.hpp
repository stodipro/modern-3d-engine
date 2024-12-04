#pragma once
#include "Component.hpp"
#include <glm/glm.hpp>

class Light : public Component {
public:
    enum class Type {
        Directional,
        Point,
        Spot
    };

    Light(Type type = Type::Directional);

    void update(float deltaTime) override;

    // Light properties
    void setType(Type type) { this->type = type; }
    void setColor(const glm::vec3& color) { this->color = color; }
    void setIntensity(float intensity) { this->intensity = intensity; }
    void setRange(float range) { this->range = range; }
    void setSpotAngle(float angle) { this->spotAngle = angle; }

    // Getters
    Type getType() const { return type; }
    const glm::vec3& getColor() const { return color; }
    float getIntensity() const { return intensity; }
    float getRange() const { return range; }
    float getSpotAngle() const { return spotAngle; }

    // Get direction (for directional and spot lights)
    glm::vec3 getDirection() const;

    // Get position (for point and spot lights)
    glm::vec3 getPosition() const;

private:
    Type type;
    glm::vec3 color;
    float intensity;
    float range;        // For point and spot lights
    float spotAngle;    // For spot lights only
};
