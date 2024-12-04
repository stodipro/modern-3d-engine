#include "ResourceManager.hpp"
#include "ModelLoader.hpp"
#include "../renderer/Shader.hpp"
#include "../renderer/Texture.hpp"
#include "../renderer/Mesh.hpp"
#include "../renderer/Material.hpp"
#include <iostream>

std::shared_ptr<Shader> ResourceManager::loadShader(const std::string& name,
                                                  const std::string& vertexPath,
                                                  const std::string& fragmentPath) {
    // Check if shader already exists
    if (shaders.find(name) != shaders.end()) {
        return shaders[name];
    }

    // Create and load new shader
    auto shader = std::make_shared<Shader>();
    if (shader->loadFromFiles(vertexPath, fragmentPath)) {
        shaders[name] = shader;
        return shader;
    }

    std::cerr << "Failed to load shader: " << name << std::endl;
    return nullptr;
}

std::shared_ptr<Shader> ResourceManager::getShader(const std::string& name) {
    auto it = shaders.find(name);
    return (it != shaders.end()) ? it->second : nullptr;
}

std::shared_ptr<Texture> ResourceManager::loadTexture(const std::string& name,
                                                    const std::string& path) {
    // Check if texture already exists
    if (textures.find(name) != textures.end()) {
        return textures[name];
    }

    // Create and load new texture
    auto texture = std::make_shared<Texture>();
    if (texture->loadFromFile(path)) {
        textures[name] = texture;
        return texture;
    }

    std::cerr << "Failed to load texture: " << name << std::endl;
    return nullptr;
}

std::shared_ptr<Texture> ResourceManager::getTexture(const std::string& name) {
    auto it = textures.find(name);
    return (it != textures.end()) ? it->second : nullptr;
}

std::shared_ptr<Mesh> ResourceManager::loadMesh(const std::string& name,
                                              const std::string& path) {
    // Check if mesh already exists
    if (meshes.find(name) != meshes.end()) {
        return meshes[name];
    }

    // Load mesh using ModelLoader
    std::vector<std::shared_ptr<Mesh>> loadedMeshes;
    if (ModelLoader::loadOBJ(path, loadedMeshes) && !loadedMeshes.empty()) {
        // Store the first mesh under the given name
        meshes[name] = loadedMeshes[0];
        
        // If there are multiple meshes, store them with indexed names
        for (size_t i = 1; i < loadedMeshes.size(); ++i) {
            meshes[name + "_" + std::to_string(i)] = loadedMeshes[i];
        }
        
        return meshes[name];
    }

    std::cerr << "Failed to load mesh: " << name << std::endl;
    return nullptr;
}

std::shared_ptr<Mesh> ResourceManager::getMesh(const std::string& name) {
    auto it = meshes.find(name);
    return (it != meshes.end()) ? it->second : nullptr;
}

std::shared_ptr<Material> ResourceManager::createMaterial(const std::string& name,
                                                        std::shared_ptr<Shader> shader) {
    // Check if material already exists
    if (materials.find(name) != materials.end()) {
        return materials[name];
    }

    // Create new material
    auto material = std::make_shared<Material>(shader);
    materials[name] = material;
    return material;
}

std::shared_ptr<Material> ResourceManager::getMaterial(const std::string& name) {
    auto it = materials.find(name);
    return (it != materials.end()) ? it->second : nullptr;
}

void ResourceManager::cleanup() {
    shaders.clear();
    textures.clear();
    meshes.clear();
    materials.clear();
}
