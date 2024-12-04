#include "MeshRenderer.hpp"
#include "Transform.hpp"
#include "../renderer/Mesh.hpp"
#include "../renderer/Material.hpp"

MeshRenderer::MeshRenderer()
    : mesh(nullptr)
    , material(nullptr)
{}

void MeshRenderer::update(float deltaTime) {
    // Update logic if needed
}

void MeshRenderer::render() {
    if (!mesh || !material) return;

    auto transform = getEntity()->getComponent<Transform>();
    if (!transform) return;

    material->bind();
    material->setModelMatrix(transform->getWorldMatrix());
    mesh->render();
}

void MeshRenderer::setMesh(std::shared_ptr<Mesh> newMesh) {
    mesh = newMesh;
}

void MeshRenderer::setMaterial(std::shared_ptr<Material> newMaterial) {
    material = newMaterial;
}
