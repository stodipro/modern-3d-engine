#include "SculptMesh.hpp"
#include <unordered_map>
#include <queue>
#include <cmath>

void SculptMesh::deformVertices(const glm::vec3& position, float radius, float strength,
                               const std::function<void(SculptVertex&, float)>& deformFunc) {
    float squaredRadius = radius * radius;

    for (auto& vertex : vertices) {
        float squaredDistance = glm::length2(vertex.position - position);
        if (squaredDistance < squaredRadius) {
            float influence = calculateFalloff(sqrt(squaredDistance), radius) * strength;
            deformFunc(vertex, influence);
        }
    }
}

float SculptMesh::calculateFalloff(float distance, float radius) {
    float x = distance / radius;
    // Smooth falloff function (1 - x^2)^2
    return (1.0f - x * x) * (1.0f - x * x);
}

void SculptMesh::subdivide(float threshold) {
    std::vector<SculptVertex> newVertices = vertices;
    std::vector<unsigned int> newIndices;
    std::unordered_map<uint64_t, unsigned int> edgeVertices;

    auto getEdgeKey = [](unsigned int a, unsigned int b) {
        return (uint64_t)std::min(a, b) << 32 | std::max(a, b);
    };

    for (size_t i = 0; i < indices.size(); i += 3) {
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];

        // Create mid-points
        unsigned int m01, m12, m20;

        // Edge 0-1
        uint64_t edge01 = getEdgeKey(i0, i1);
        auto it01 = edgeVertices.find(edge01);
        if (it01 == edgeVertices.end()) {
            SculptVertex midVertex;
            midVertex.position = (vertices[i0].position + vertices[i1].position) * 0.5f;
            midVertex.normal = glm::normalize(vertices[i0].normal + vertices[i1].normal);
            midVertex.texCoords = (vertices[i0].texCoords + vertices[i1].texCoords) * 0.5f;

            m01 = newVertices.size();
            newVertices.push_back(midVertex);
            edgeVertices[edge01] = m01;
        } else {
            m01 = it01->second;
        }

        // Edge 1-2
        uint64_t edge12 = getEdgeKey(i1, i2);
        auto it12 = edgeVertices.find(edge12);
        if (it12 == edgeVertices.end()) {
            SculptVertex midVertex;
            midVertex.position = (vertices[i1].position + vertices[i2].position) * 0.5f;
            midVertex.normal = glm::normalize(vertices[i1].normal + vertices[i2].normal);
            midVertex.texCoords = (vertices[i1].texCoords + vertices[i2].texCoords) * 0.5f;

            m12 = newVertices.size();
            newVertices.push_back(midVertex);
            edgeVertices[edge12] = m12;
        } else {
            m12 = it12->second;
        }

        // Edge 2-0
        uint64_t edge20 = getEdgeKey(i2, i0);
        auto it20 = edgeVertices.find(edge20);
        if (it20 == edgeVertices.end()) {
            SculptVertex midVertex;
            midVertex.position = (vertices[i2].position + vertices[i0].position) * 0.5f;
            midVertex.normal = glm::normalize(vertices[i2].normal + vertices[i0].normal);
            midVertex.texCoords = (vertices[i2].texCoords + vertices[i0].texCoords) * 0.5f;

            m20 = newVertices.size();
            newVertices.push_back(midVertex);
            edgeVertices[edge20] = m20;
        } else {
            m20 = it20->second;
        }

        // Create four new triangles
        newIndices.push_back(i0);  newIndices.push_back(m01); newIndices.push_back(m20);
        newIndices.push_back(m01); newIndices.push_back(i1);  newIndices.push_back(m12);
        newIndices.push_back(m20); newIndices.push_back(m12); newIndices.push_back(i2);
        newIndices.push_back(m01); newIndices.push_back(m12); newIndices.push_back(m20);
    }

    // Update the mesh with new data
    vertices = std::move(newVertices);
    indices = std::move(newIndices);

    // Update vertex adjacency information
    updateAdjacencyInfo();
}

void SculptMesh::updateAdjacencyInfo() {
    // Clear existing adjacency info
    for (auto& vertex : vertices) {
        vertex.adjacentVertices.clear();
    }

    // Build adjacency from triangles
    for (size_t i = 0; i < indices.size(); i += 3) {
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];

        vertices[i0].adjacentVertices.push_back(i1);
        vertices[i0].adjacentVertices.push_back(i2);
        vertices[i1].adjacentVertices.push_back(i0);
        vertices[i1].adjacentVertices.push_back(i2);
        vertices[i2].adjacentVertices.push_back(i0);
        vertices[i2].adjacentVertices.push_back(i1);
    }

    // Remove duplicates in adjacency lists
    for (auto& vertex : vertices) {
        std::sort(vertex.adjacentVertices.begin(), vertex.adjacentVertices.end());
        vertex.adjacentVertices.erase(
            std::unique(vertex.adjacentVertices.begin(), vertex.adjacentVertices.end()),
            vertex.adjacentVertices.end());
    }
}
