#pragma once
#include "SculptMesh.hpp"
#include <string>

class MeshIO {
public:
    // OBJ format
    static bool saveToOBJ(const SculptMesh& mesh, const std::string& filepath);
    static bool loadFromOBJ(SculptMesh& mesh, const std::string& filepath);

    // Custom binary format for faster loading/saving
    static bool saveToBinary(const SculptMesh& mesh, const std::string& filepath);
    static bool loadFromBinary(SculptMesh& mesh, const std::string& filepath);

private:
    // Binary format header
    struct BinaryHeader {
        char magic[4] = {'S', 'M', 'S', 'H'}; // Sculpt Mesh
        uint32_t version = 1;
        uint32_t vertexCount = 0;
        uint32_t indexCount = 0;
    };
};
