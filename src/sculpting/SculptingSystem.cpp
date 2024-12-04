#include "SculptingSystem.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "../components/Transform.hpp"

void SculptingSystem::initialize() {
    activeCamera = nullptr;
    targetMesh = nullptr;
}

void SculptingSystem::update(float deltaTime) {
    if (!targetMesh || !activeCamera) return;

    auto& input = Input::getInstance();

    // Handle tool size adjustment
    if (input.getMouseScrollDelta() != 0.0f) {
        float newRadius = activeTool.getRadius() * (1.0f + input.getMouseScrollDelta() * 0.1f);
        setToolRadius(glm::clamp(newRadius, 0.1f, 10.0f));
    }

    // Handle sculpting input
    if (input.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        glm::vec3 hitPoint;
        if (castRayToMesh(input.getMousePosition(), hitPoint)) {
            activeTool.apply(*targetMesh, hitPoint);
        }
    }
}

void SculptingSystem::setActiveTool(SculptingTool::Type type) {
    activeTool.setType(type);
}

void SculptingSystem::setToolRadius(float radius) {
    activeTool.setRadius(radius);
}

void SculptingSystem::setToolStrength(float strength) {
    activeTool.setStrength(strength);
}

bool SculptingSystem::castRayToMesh(const glm::vec2& screenPos, glm::vec3& hitPoint) {
    if (!targetMesh || !activeCamera) return false;

    glm::vec3 rayDir = screenToWorldRay(screenPos);
    glm::vec3 rayOrigin = activeCamera->getEntity()->getComponent<Transform>()->getPosition();

    // Simple mesh intersection test
    float nearestDistance = std::numeric_limits<float>::max();
    bool hit = false;

    // Test against each triangle in the mesh
    const auto& vertices = targetMesh->getVertices();
    const auto& indices = targetMesh->getIndices();

    for (size_t i = 0; i < indices.size(); i += 3) {
        const glm::vec3& v0 = vertices[indices[i]].position;
        const glm::vec3& v1 = vertices[indices[i + 1]].position;
        const glm::vec3& v2 = vertices[indices[i + 2]].position;

        // Calculate triangle normal and plane distance
        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
        float d = -glm::dot(normal, v0);

        // Ray-plane intersection
        float denom = glm::dot(normal, rayDir);
        if (abs(denom) > 0.0001f) { // Avoid division by zero
            float t = -(glm::dot(normal, rayOrigin) + d) / denom;
            if (t >= 0 && t < nearestDistance) {
                // Calculate intersection point
                glm::vec3 p = rayOrigin + t * rayDir;

                // Check if point is inside triangle
                glm::vec3 edge0 = v1 - v0;
                glm::vec3 edge1 = v2 - v1;
                glm::vec3 edge2 = v0 - v2;
                glm::vec3 c0 = p - v0;
                glm::vec3 c1 = p - v1;
                glm::vec3 c2 = p - v2;

                if (glm::dot(normal, glm::cross(edge0, c0)) > 0 &&
                    glm::dot(normal, glm::cross(edge1, c1)) > 0 &&
                    glm::dot(normal, glm::cross(edge2, c2)) > 0) {
                    nearestDistance = t;
                    hitPoint = p;
                    hit = true;
                }
            }
        }
    }

    return hit;
}

glm::vec3 SculptingSystem::screenToWorldRay(const glm::vec2& screenPos) {
    if (!activeCamera) return glm::vec3(0.0f);

    // Get window dimensions
    GLFWwindow* window = glfwGetCurrentContext();
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Convert to normalized device coordinates
    glm::vec2 ndc(
        (2.0f * screenPos.x) / width - 1.0f,
        1.0f - (2.0f * screenPos.y) / height
    );

    // Get view and projection matrices
    glm::mat4 projection = activeCamera->getProjectionMatrix();
    glm::mat4 view = activeCamera->getViewMatrix();

    // Calculate inverse view-projection matrix
    glm::mat4 invVP = glm::inverse(projection * view);

    // Transform to world space
    glm::vec4 rayClip = glm::vec4(ndc.x, ndc.y, -1.0f, 1.0f);
    glm::vec4 rayEye = glm::inverse(projection) * rayClip;
    rayEye.z = -1.0f;
    rayEye.w = 0.0f;

    glm::vec3 rayWorld = glm::normalize(glm::vec3(glm::inverse(view) * rayEye));
    return rayWorld;
}
