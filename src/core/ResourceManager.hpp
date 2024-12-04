#pragma once
#include <string>
#include <memory>
#include <unordered_map>

class Shader;
class Texture;
class Mesh;
class Material;

class ResourceManager {
public:
    static ResourceManager& getInstance() {
        static ResourceManager instance;
        return instance;
    }

    // Shader management
    std::shared_ptr<Shader> loadShader(const std::string& name,
                                     const std::string& vertexPath,
                                     const std::string& fragmentPath);
    std::shared_ptr<Shader> getShader(const std::string& name);

    // Texture management
    std::shared_ptr<Texture> loadTexture(const std::string& name,
                                       const std::string& path);
    std::shared_ptr<Texture> getTexture(const std::string& name);

    // Mesh management
    std::shared_ptr<Mesh> loadMesh(const std::string& name,
                                  const std::string& path);
    std::shared_ptr<Mesh> getMesh(const std::string& name);

    // Material management
    std::shared_ptr<Material> createMaterial(const std::string& name,
                                           std::shared_ptr<Shader> shader);
    std::shared_ptr<Material> getMaterial(const std::string& name);

    // Resource cleanup
    void cleanup();

private:
    ResourceManager() = default;
    ~ResourceManager() = default;
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    // Resource caches
    std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;
    std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
    std::unordered_map<std::string, std::shared_ptr<Mesh>> meshes;
    std::unordered_map<std::string, std::shared_ptr<Material>> materials;
};
