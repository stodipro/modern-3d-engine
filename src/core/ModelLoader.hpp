#pragma once
#include <string>
#include <memory>
#include <vector>
#include <glm/glm.hpp>

class Mesh;

class ModelLoader {
public:
    static bool loadOBJ(const std::string& path, 
                       std::vector<std::shared_ptr<Mesh>>& outMeshes);

private:
    struct OBJData {
        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texCoords;
        std::vector<unsigned int> indices;
    };

    static bool parseOBJFile(const std::string& path, OBJData& data);
    static void calculateNormals(OBJData& data);
};
