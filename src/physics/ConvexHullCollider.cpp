#include "ConvexHullCollider.hpp"
#include <map>
#include <set>

void ConvexHullCollider::buildConvexHull() {
    if (vertices.size() < 4) return;

    // Initialize with tetrahedron
    faces.push_back(glm::uvec3(0, 1, 2));
    faces.push_back(glm::uvec3(0, 2, 3));
    faces.push_back(glm::uvec3(0, 3, 1));
    faces.push_back(glm::uvec3(1, 3, 2));

    // Incrementally add remaining points
    for (size_t i = 4; i < vertices.size(); i++) {
        addPointToHull(vertices[i]);
    }

    // Calculate face normals
    updateNormals();
}

void ConvexHullCollider::addPointToHull(const glm::vec3& point) {
    std::vector<size_t> visibleFaces;

    // Find faces visible from point
    for (size_t i = 0; i < faces.size(); i++) {
        glm::vec3 normal = glm::normalize(glm::cross(
            vertices[faces[i].y] - vertices[faces[i].x],
            vertices[faces[i].z] - vertices[faces[i].x]
        ));

        if (glm::dot(point - vertices[faces[i].x], normal) > 0.0001f) {
            visibleFaces.push_back(i);
        }
    }

    if (visibleFaces.empty()) return;

    // Find horizon edges
    std::vector<std::pair<size_t, size_t>> horizon;
    findHorizonEdges(visibleFaces, horizon);

    // Remove visible faces
    for (auto it = visibleFaces.rbegin(); it != visibleFaces.rend(); ++it) {
        faces[*it] = faces.back();
        faces.pop_back();
    }

    // Add new faces connecting point to horizon
    size_t pointIndex = vertices.size();
    vertices.push_back(point);

    for (const auto& edge : horizon) {
        faces.push_back(glm::uvec3(edge.first, edge.second, pointIndex));
    }
}

void ConvexHullCollider::findHorizonEdges(const std::vector<size_t>& visibleFaces,
                                         std::vector<std::pair<size_t, size_t>>& horizon) {
    std::map<std::pair<size_t, size_t>, int> edges;

    // Count edge occurrences
    for (size_t faceIdx : visibleFaces) {
        const auto& face = faces[faceIdx];
        std::pair<size_t, size_t> e1(std::min(face.x, face.y), std::max(face.x, face.y));
        std::pair<size_t, size_t> e2(std::min(face.y, face.z), std::max(face.y, face.z));
        std::pair<size_t, size_t> e3(std::min(face.z, face.x), std::max(face.z, face.x));

        edges[e1]++;
        edges[e2]++;
        edges[e3]++;
    }

    // Find edges that appear only once (horizon)
    for (const auto& edge : edges) {
        if (edge.second == 1) {
            horizon.push_back(edge.first);
        }
    }
}

void ConvexHullCollider::updateNormals() {
    normals.clear();
    normals.reserve(faces.size());

    for (const auto& face : faces) {
        glm::vec3 normal = glm::normalize(glm::cross(
            vertices[face.y] - vertices[face.x],
            vertices[face.z] - vertices[face.x]
        ));
        normals.push_back(normal);
    }
}
