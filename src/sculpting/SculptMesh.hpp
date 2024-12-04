#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "../renderer/Mesh.hpp"

class SculptMesh {
public:
    SculptMesh();
    ~SculptMesh();

    // Basic mesh operations
    void setVertices(const std::vector<Vertex>& vertices) { this->vertices = vertices; }
    void setIndices(const std::vector<unsigned int>& indices) { this->indices = indices; }
    const std::vector<Vertex>& getVertices() const { return vertices; }
    const std::vector<unsigned int>& getIndices() const { return indices; }

    // Mesh generation
    void initializeAsSphere(float radius, int subdivisions);
    void initializeAsCube(float size, int subdivisions);
    void initializeAsPlane(float width, float height, int subdivisions);

    // Basic sculpting operations
    void pull(const glm::vec3& position, float radius, float strength);
    void push(const glm::vec3& position, float radius, float strength);
    void smooth(const glm::vec3& position, float radius, float strength);
    void pinch(const glm::vec3& position, float radius, float strength);

    // Advanced sculpting operations
    void flatten(const glm::vec3& position, float radius, float strength, const glm::vec3& normal);
    void crease(const glm::vec3& position, float radius, float strength, const glm::vec3& direction);
    void inflate(const glm::vec3& position, float radius, float strength);
    void scrape(const glm::vec3& position, float radius, float strength, const glm::vec3& direction);
    void twist(const glm::vec3& position, float radius, float strength, const glm::vec3& axis);
    void clay(const glm::vec3& position, float radius, float strength);

    // Mesh operations
    void recalculateNormals();
    void subdivide(float threshold);
    std::shared_ptr<Mesh> generateMesh();
    void updateAdjacencyInfo();

private:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Helper functions
    void deformVertices(const glm::vec3& position, float radius, float strength,
                       const std::function<void(Vertex&, float)>& deformFunc);
    float calculateFalloff(float distance, float radius);
};
