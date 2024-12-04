#pragma once
#include "Component.hpp"
#include <glm/glm.hpp>

class Camera : public Component {
public:
    enum class ProjectionType {
        Perspective,
        Orthographic
    };

    Camera();
    void update(float deltaTime) override;

    // Perspective settings
    void setPerspective(float fovDegrees, float aspectRatio, float nearPlane, float farPlane);
    void setOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);

    // Camera properties
    void setProjectionType(ProjectionType type);
    void setFOV(float degrees);
    void setAspectRatio(float ratio);
    void setNearPlane(float near);
    void setFarPlane(float far);

    // Matrices
    const glm::mat4& getProjectionMatrix() const;
    glm::mat4 getViewMatrix() const;
    
    // Frustum properties
    float getFOV() const { return fov; }
    float getAspectRatio() const { return aspectRatio; }
    float getNearPlane() const { return nearPlane; }
    float getFarPlane() const { return farPlane; }

private:
    ProjectionType projType;
    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;

    // Orthographic properties
    float orthoLeft;
    float orthoRight;
    float orthoBottom;
    float orthoTop;

    glm::mat4 projectionMatrix;
    bool dirtyProjection;

    void updateProjectionMatrix();
};
