#include "SculptMesh.hpp"
#include <glm/gtx/rotate_vector.hpp>

// ... (keeping existing implementations) ...

void SculptMesh::flatten(const glm::vec3& position, float radius, float strength, const glm::vec3& normal) {
    glm::vec3 targetPlaneNormal = glm::normalize(normal);
    
    deformVertices(position, radius, strength, [&](Vertex& vertex, float influence) {
        float dist = glm::dot(vertex.position - position, targetPlaneNormal);
        vertex.position -= targetPlaneNormal * dist * influence;
    });

    recalculateNormals();
}

void SculptMesh::crease(const glm::vec3& position, float radius, float strength, const glm::vec3& direction) {
    glm::vec3 creaseDir = glm::normalize(direction);
    
    deformVertices(position, radius, strength, [&](Vertex& vertex, float influence) {
        glm::vec3 toVertex = vertex.position - position;
        float dist = glm::length(toVertex - creaseDir * glm::dot(toVertex, creaseDir));
        vertex.position -= (toVertex - creaseDir * glm::dot(toVertex, creaseDir)) * influence;
    });

    recalculateNormals();
}

void SculptMesh::inflate(const glm::vec3& position, float radius, float strength) {
    deformVertices(position, radius, strength, [&](Vertex& vertex, float influence) {
        glm::vec3 toVertex = vertex.position - position;
        float dist = glm::length(toVertex);
        if (dist > 0.0001f) {
            vertex.position += (toVertex / dist) * influence;
        }
    });

    recalculateNormals();
}

void SculptMesh::scrape(const glm::vec3& position, float radius, float strength, const glm::vec3& direction) {
    glm::vec3 scrapeDir = glm::normalize(direction);
    float maxDepth = radius * 0.5f;

    deformVertices(position, radius, strength, [&](Vertex& vertex, float influence) {
        glm::vec3 toVertex = vertex.position - position;
        float depth = glm::dot(toVertex, scrapeDir);
        if (depth > -maxDepth) {
            vertex.position -= scrapeDir * (depth + maxDepth) * influence;
        }
    });

    recalculateNormals();
}

void SculptMesh::twist(const glm::vec3& position, float radius, float strength, const glm::vec3& axis) {
    glm::vec3 twistAxis = glm::normalize(axis);
    
    deformVertices(position, radius, strength, [&](Vertex& vertex, float influence) {
        glm::vec3 toVertex = vertex.position - position;
        float angle = strength * influence * glm::pi<float>();
        vertex.position = position + glm::rotate(toVertex, angle, twistAxis);
    });

    recalculateNormals();
}

void SculptMesh::clay(const glm::vec3& position, float radius, float strength) {
    float clayHeight = radius * 0.2f;
    
    deformVertices(position, radius, strength, [&](Vertex& vertex, float influence) {
        glm::vec3 toVertex = vertex.position - position;
        float dist = glm::length(toVertex);
        if (dist > 0.0001f) {
            float heightFactor = 1.0f - (dist / radius);
            heightFactor = glm::max(0.0f, heightFactor);
            vertex.position += vertex.normal * clayHeight * heightFactor * influence;
        }
    });

    recalculateNormals();
}
