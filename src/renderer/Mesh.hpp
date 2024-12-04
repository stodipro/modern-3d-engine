#pragma once
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

struct SubMesh {
    unsigned int baseVertex;
    unsigned int baseIndex;
    unsigned int numIndices;
    unsigned int materialIndex;
};

class Mesh {
public:
    Mesh();
    ~Mesh();

    void initialize(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
    void render() const;
    void cleanup();

    // Getters for mesh data
    const std::vector<Vertex>& getVertices() const { return vertices; }
    const std::vector<uint32_t>& getIndices() const { return indices; }
    const std::vector<SubMesh>& getSubMeshes() const { return subMeshes; }

    // Add submesh for multi-material support
    void addSubMesh(const SubMesh& subMesh) { subMeshes.push_back(subMesh); }

private:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<SubMesh> subMeshes;

    GLuint VAO;
    GLuint VBO;
    GLuint EBO;

    void setupMesh();
};
