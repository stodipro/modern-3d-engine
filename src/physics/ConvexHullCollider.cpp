#include "ConvexHullCollider.hpp"
#include "../components/Transform.hpp"
#include <algorithm>
#include <set>
#include <map>

ConvexHullCollider::ConvexHullCollider()
    : Collider(Type::ConvexHull)
{}

bool ConvexHullCollider::gjk(const ConvexHullCollider* other, Simplex& simplex) const {
    // Initial direction
    glm::vec3 direction = other->getSupport(glm::vec3(0, 1, 0)) - getSupport(glm::vec3(0, 1, 0));
    
    // Get first point for simplex
    simplex.points[0] = support(other, direction);
    if (glm::dot(simplex.points[0], direction) <= 0) {
        return false;
    }

    // Second direction is towards the origin
    direction = -simplex.points[0];
    simplex.size = 1;

    while (true) {
        glm::vec3 newPoint = support(other, direction);
        
        if (glm::dot(newPoint, direction) <= 0) {
            return false;
        }

        simplex.points[simplex.size++] = newPoint;

        if (handleSimplex(simplex, direction)) {
            return true;
        }
    }
}

glm::vec3 ConvexHullCollider::support(const ConvexHullCollider* other, const glm::vec3& direction) const {
    return getSupport(direction) - other->getSupport(-direction);
}

bool ConvexHullCollider::handleSimplex(Simplex& simplex, glm::vec3& direction) const {
    switch (simplex.size) {
        case 2: return handleLine(simplex, direction);
        case 3: return handleTriangle(simplex, direction);
        case 4: return handleTetrahedron(simplex, direction);
    }
    return false;
}

bool ConvexHullCollider::handleLine(Simplex& simplex, glm::vec3& direction) const {
    glm::vec3 a = simplex.points[1];
    glm::vec3 b = simplex.points[0];
    glm::vec3 ab = b - a;
    glm::vec3 ao = -a;

    if (glm::dot(ab, ao) > 0) {
        direction = glm::cross(glm::cross(ab, ao), ab);
    } else {
        simplex.points[0] = a;
        simplex.size = 1;
        direction = ao;
    }

    return false;
}

bool ConvexHullCollider::handleTriangle(Simplex& simplex, glm::vec3& direction) const {
    glm::vec3 a = simplex.points[2];
    glm::vec3 b = simplex.points[1];
    glm::vec3 c = simplex.points[0];

    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;
    glm::vec3 ao = -a;

    glm::vec3 abc = glm::cross(ab, ac);

    if (glm::dot(glm::cross(abc, ac), ao) > 0) {
        if (glm::dot(ac, ao) > 0) {
            simplex.points[0] = a;
            simplex.points[1] = c;
            simplex.size = 2;
            direction = glm::cross(glm::cross(ac, ao), ac);
        } else {
            simplex.points[0] = a;
            simplex.points[1] = b;
            simplex.size = 2;
            direction = glm::cross(glm::cross(ab, ao), ab);
        }
    } else {
        if (glm::dot(glm::cross(ab, abc), ao) > 0) {
            simplex.points[0] = a;
            simplex.points[1] = b;
            simplex.size = 2;
            direction = glm::cross(glm::cross(ab, ao), ab);
        } else {
            if (glm::dot(abc, ao) > 0) {
                direction = abc;
            } else {
                simplex.points[0] = a;
                simplex.points[1] = c;
                simplex.points[2] = b;
                direction = -abc;
            }
        }
    }

    return false;
}

bool ConvexHullCollider::handleTetrahedron(Simplex& simplex, glm::vec3& direction) const {
    glm::vec3 a = simplex.points[3];
    glm::vec3 b = simplex.points[2];
    glm::vec3 c = simplex.points[1];
    glm::vec3 d = simplex.points[0];

    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;
    glm::vec3 ad = d - a;
    glm::vec3 ao = -a;

    glm::vec3 abc = glm::cross(ab, ac);
    glm::vec3 acd = glm::cross(ac, ad);
    glm::vec3 adb = glm::cross(ad, ab);

    if (glm::dot(abc, ao) > 0) {
        simplex.points[0] = a;
        simplex.points[1] = b;
        simplex.points[2] = c;
        simplex.size = 3;
        direction = abc;
        return false;
    }

    if (glm::dot(acd, ao) > 0) {
        simplex.points[0] = a;
        simplex.points[1] = c;
        simplex.points[2] = d;
        simplex.size = 3;
        direction = acd;
        return false;
    }

    if (glm::dot(adb, ao) > 0) {
        simplex.points[0] = a;
        simplex.points[1] = d;
        simplex.points[2] = b;
        simplex.size = 3;
        direction = adb;
        return false;
    }

    return true;
}

glm::vec3 ConvexHullCollider::epa(const ConvexHullCollider* other, const Simplex& initialSimplex) const {
    std::vector<glm::vec3> polytope(initialSimplex.points, initialSimplex.points + initialSimplex.size);
    std::vector<glm::uvec3> faces;
    std::vector<glm::vec3> normals;

    // Build initial faces from simplex
    buildInitialFaces(polytope, faces, normals);

    float minDistance = FLT_MAX;
    glm::vec3 minNormal;
    size_t minFace = 0;

    const float TOLERANCE = 0.0001f;
    const int MAX_ITERATIONS = 32;
    int iteration = 0;

    while (iteration++ < MAX_ITERATIONS) {
        // Find face closest to origin
        minDistance = FLT_MAX;
        for (size_t i = 0; i < faces.size(); i++) {
            float dist = glm::dot(normals[i], polytope[faces[i].x]);
            if (dist < minDistance) {
                minDistance = dist;
                minNormal = normals[i];
                minFace = i;
            }
        }

        // Get support point in minNormal direction
        glm::vec3 supportPoint = support(other, minNormal);
        float newDistance = glm::dot(minNormal, supportPoint);

        // Check if we've found the edge
        if (std::abs(newDistance - minDistance) < TOLERANCE) {
            return minNormal * minDistance;
        }

        // Add new point and update polytope
        polytope.push_back(supportPoint);
        expandPolytopeWithPoint(polytope, faces, normals, polytope.size() - 1);
    }

    return minNormal * minDistance;
}

void ConvexHullCollider::buildInitialFaces(std::vector<glm::vec3>& polytope,
                                          std::vector<glm::uvec3>& faces,
                                          std::vector<glm::vec3>& normals) const {
    // Assuming we have a tetrahedron
    faces = {
        glm::uvec3(0, 1, 2),
        glm::uvec3(0, 2, 3),
        glm::uvec3(0, 3, 1),
        glm::uvec3(1, 3, 2)
    };

    for (const auto& face : faces) {
        glm::vec3 normal = glm::normalize(glm::cross(
            polytope[face.y] - polytope[face.x],
            polytope[face.z] - polytope[face.x]
        ));
        normals.push_back(normal);
    }
}

void ConvexHullCollider::expandPolytopeWithPoint(std::vector<glm::vec3>& polytope,
                                                std::vector<glm::uvec3>& faces,
                                                std::vector<glm::vec3>& normals,
                                                size_t newPointIndex) const {
    std::vector<size_t> visibleFaces;
    glm::vec3 newPoint = polytope[newPointIndex];

    // Find visible faces
    for (size_t i = 0; i < faces.size(); i++) {
        if (glm::dot(normals[i], newPoint - polytope[faces[i].x]) > 0) {
            visibleFaces.push_back(i);
        }
    }

    // Find boundary edges
    std::vector<std::pair<size_t, size_t>> boundary;
    findBoundaryEdges(faces, visibleFaces, boundary);

    // Remove visible faces
    removeFaces(faces, normals, visibleFaces);

    // Add new faces
    for (const auto& edge : boundary) {
        glm::uvec3 newFace(edge.first, edge.second, newPointIndex);
        glm::vec3 normal = glm::normalize(glm::cross(
            polytope[newFace.y] - polytope[newFace.x],
            polytope[newFace.z] - polytope[newFace.x]
        ));
        
        faces.push_back(newFace);
        normals.push_back(normal);
    }
}
