#pragma once
#include "Component.hpp"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

struct Mesh;
class Material;

class MeshRenderer : public Component {
public:
    MeshRenderer();
    void update(float deltaTime) override;
    void render() override;

    void setMesh(std::shared_ptr<Mesh> mesh);
    void setMaterial(std::shared_ptr<Material> material);

    Mesh* getMesh() const { return mesh.get(); }
    Material* getMaterial() const { return material.get(); }

private:
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;
};
