#include "MeshIO.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

bool MeshIO::saveToOBJ(const SculptMesh& mesh, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filepath << std::endl;
        return false;
    }

    // Write header comment
    file << "# Exported from Modern 3D Engine Sculpting System\n";
    file << "# Vertices: " << mesh.getVertices().size() << "\n";
    file << "# Faces: " << mesh.getIndices().size() / 3 << "\n\n";

    // Write vertices
    for (const auto& vertex : mesh.getVertices()) {
        file << "v " << std::fixed << std::setprecision(6)
             << vertex.position.x << " "
             << vertex.position.y << " "
             << vertex.position.z << "\n";
    }

    // Write texture coordinates
    for (const auto& vertex : mesh.getVertices()) {
        file << "vt " << std::fixed << std::setprecision(6)
             << vertex.texCoords.x << " "
             << vertex.texCoords.y << "\n";
    }

    // Write normals
    for (const auto& vertex : mesh.getVertices()) {
        file << "vn " << std::fixed << std::setprecision(6)
             << vertex.normal.x << " "
             << vertex.normal.y << " "
             << vertex.normal.z << "\n";
    }

    // Write faces
    for (size_t i = 0; i < mesh.getIndices().size(); i += 3) {
        file << "f ";
        for (size_t j = 0; j < 3; ++j) {
            unsigned int idx = mesh.getIndices()[i + j] + 1; // OBJ indices are 1-based
            file << idx << "/" << idx << "/" << idx << " ";
        }
        file << "\n";
    }

    return true;
}

bool MeshIO::loadFromOBJ(SculptMesh& mesh, const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return false;
    }

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v") {
            glm::vec3 pos;
            iss >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);
        }
        else if (type == "vt") {
            glm::vec2 tex;
            iss >> tex.x >> tex.y;
            texCoords.push_back(tex);
        }
        else if (type == "vn") {
            glm::vec3 norm;
            iss >> norm.x >> norm.y >> norm.z;
            normals.push_back(norm);
        }
        else if (type == "f") {
            // Parse face indices
            for (int i = 0; i < 3; ++i) {
                std::string vertexData;
                iss >> vertexData;
                
                size_t pos = vertexData.find('/');
                if (pos != std::string::npos) {
                    indices.push_back(std::stoi(vertexData.substr(0, pos)) - 1);
                }
            }
        }
    }

    // Create vertices
    std::vector<Vertex> vertices;
    for (size_t i = 0; i < positions.size(); ++i) {
        Vertex vertex;
        vertex.position = positions[i];
        vertex.texCoords = i < texCoords.size() ? texCoords[i] : glm::vec2(0.0f);
        vertex.normal = i < normals.size() ? normals[i] : glm::vec3(0.0f, 1.0f, 0.0f);
        vertices.push_back(vertex);
    }

    // Update mesh
    mesh.setVertices(vertices);
    mesh.setIndices(indices);
    mesh.recalculateNormals();

    return true;
}

bool MeshIO::saveToBinary(const SculptMesh& mesh, const std::string& filepath) {
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filepath << std::endl;
        return false;
    }

    // Prepare header
    BinaryHeader header;
    header.vertexCount = static_cast<uint32_t>(mesh.getVertices().size());
    header.indexCount = static_cast<uint32_t>(mesh.getIndices().size());

    // Write header
    file.write(reinterpret_cast<const char*>(&header), sizeof(BinaryHeader));

    // Write vertex data
    file.write(reinterpret_cast<const char*>(mesh.getVertices().data()),
               sizeof(Vertex) * header.vertexCount);

    // Write index data
    file.write(reinterpret_cast<const char*>(mesh.getIndices().data()),
               sizeof(unsigned int) * header.indexCount);

    return true;
}

bool MeshIO::loadFromBinary(SculptMesh& mesh, const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return false;
    }

    // Read and verify header
    BinaryHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(BinaryHeader));

    if (std::string(header.magic, 4) != "SMSH") {
        std::cerr << "Invalid file format" << std::endl;
        return false;
    }

    // Read vertex data
    std::vector<Vertex> vertices(header.vertexCount);
    file.read(reinterpret_cast<char*>(vertices.data()),
              sizeof(Vertex) * header.vertexCount);

    // Read index data
    std::vector<unsigned int> indices(header.indexCount);
    file.read(reinterpret_cast<char*>(indices.data()),
              sizeof(unsigned int) * header.indexCount);

    // Update mesh
    mesh.setVertices(vertices);
    mesh.setIndices(indices);
    mesh.recalculateNormals();

    return true;
}
