#pragma once
#include <glm/glm.hpp>
#include "SculptMesh.hpp"

class SculptingTool {
public:
    enum class Type {
        Pull,
        Push,
        Smooth,
        Pinch
    };

    SculptingTool();

    void setType(Type type) { this->type = type; }
    void setRadius(float radius) { this->radius = radius; }
    void setStrength(float strength) { this->strength = strength; }

    // Apply the tool at a specific position
    void apply(SculptMesh& mesh, const glm::vec3& position);

    // Getters
    Type getType() const { return type; }
    float getRadius() const { return radius; }
    float getStrength() const { return strength; }

private:
    Type type;
    float radius;
    float strength;
};
