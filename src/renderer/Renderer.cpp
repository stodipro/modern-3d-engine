#include "Renderer.hpp"
#include "../scene/Scene.hpp"

Renderer::Renderer() : clearColor(0.2f, 0.3f, 0.3f, 1.0f) {}

Renderer::~Renderer() {
    shutdown();
}

bool Renderer::initialize() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    return true;
}

void Renderer::shutdown() {
    // Clean up any renderer resources
}

void Renderer::beginFrame() {
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::render(const Scene& scene) {
    // Will be implemented when we add scene rendering
}

void Renderer::endFrame() {
    // Perform any end-of-frame operations
}

void Renderer::setViewport(int width, int height) {
    glViewport(0, 0, width, height);
}

void Renderer::setClearColor(const glm::vec4& color) {
    clearColor = color;
}
