#include "ModelLoader.hpp"
#include "../renderer/Mesh.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

bool ModelLoader::loadOBJ(const std::string& path, 
                         std::vector<std::shared_ptr<Mesh>>& outMeshes) {
    OBJData data;
    if (!parseOBJFile(path, data)) {
        return false;
    }

    // If no normals were provided, calculate them
    if (data.normals.empty()) {
        calculateNormals(data);
    }

    // Create vertex array
    std::vector<Vertex> vertices;
    for (size_t i = 0; i < data.positions.size(); ++i) {
        Vertex vertex;
        vertex.position = data.positions[i];
        vertex.normal = i < data.normals.size() ? data.normals[i] : glm::vec3(0.0f, 1.0f, 0.0f);
        vertex.texCoords = i < data.texCoords.size() ? data.texCoords[i] : glm::vec2(0.0f);
        vertices.push_back(vertex);
    }

    // Create mesh
    auto mesh = std::make_shared<Mesh>();
    mesh->initialize(vertices, data.indices);
    outMeshes.push_back(mesh);

    return true;
}

bool ModelLoader::parseOBJFile(const std::string& path, OBJData& data) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << path << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v") {
            // Vertex position
            glm::vec3 pos;
            iss >> pos.x >> pos.y >> pos.z;
            data.positions.push_back(pos);
        }
        else if (type == "vt") {
            // Texture coordinate
            glm::vec2 tex;
            iss >> tex.x >> tex.y;
            data.texCoords.push_back(tex);
        }
        else if (type == "vn") {
            // Vertex normal
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            data.normals.push_back(normal);
        }
        else if (type == "f") {
            // Face
            std::string vertex;
            while (iss >> vertex) {
                // Parse face indices (format: vertex/texture/normal)
                std::istringstream viss(vertex);
                std::string index;
                std::getline(viss, index, '/');
                if (!index.empty()) {
                    data.indices.push_back(std::stoi(index) - 1);
                }
            }
        }
    }

    return !data.positions.empty();
}

void ModelLoader::calculateNormals(OBJData& data) {
    data.normals.resize(data.positions.size(), glm::vec3(0.0f));

    // Calculate normals for each triangle
    for (size_t i = 0; i < data.indices.size(); i += 3) {
        unsigned int i0 = data.indices[i];
        unsigned int i1 = data.indices[i + 1];
        unsigned int i2 = data.indices[i + 2];

        glm::vec3 v1 = data.positions[i1] - data.positions[i0];
        glm::vec3 v2 = data.positions[i2] - data.positions[i0];
        glm::vec3 normal = glm::normalize(glm::cross(v1, v2));

        // Add the normal to all three vertices of the triangle
        data.normals[i0] += normal;
        data.normals[i1] += normal;
        data.normals[i2] += normal;
    }

    // Normalize all normals
    for (auto& normal : data.normals) {
        normal = glm::normalize(normal);
    }
}
