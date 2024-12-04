#pragma once
#include "Component.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

class Transform : public Component {
public:
    Transform();
    void update(float deltaTime) override;

    // Position
    void setPosition(const glm::vec3& pos);
    const glm::vec3& getPosition() const;
    void translate(const glm::vec3& delta);

    // Rotation
    void setRotation(const glm::quat& rot);
    void setRotationEuler(const glm::vec3& euler);
    const glm::quat& getRotation() const;
    glm::vec3 getRotationEuler() const;
    void rotate(const glm::vec3& euler);

    // Scale
    void setScale(const glm::vec3& scale);
    const glm::vec3& getScale() const;

    // Matrices
    glm::mat4 getLocalMatrix() const;
    glm::mat4 getWorldMatrix() const;

    // Hierarchy
    void setParent(Transform* parent);
    Transform* getParent() const;

private:
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
    Transform* parent;
    bool dirtyMatrix;
    glm::mat4 cachedWorldMatrix;

    void updateWorldMatrix();
};
