#include "DemoScene.hpp"
#include "../src/core/ResourceManager.hpp"
#include "../src/components/Transform.hpp"
#include "../src/components/MeshRenderer.hpp"
#include <GLFW/glfw3.h>

DemoScene::DemoScene() {}

DemoScene::~DemoScene() {}

bool DemoScene::initialize() {
    if (!engine.initialize()) {
        return false;
    }

    // Initialize input system
    Input::getInstance().initialize(engine.getWindow());

    // Setup scene
    setupCamera();
    setupModel();

    return true;
}

void DemoScene::setupCamera() {
    cameraEntity = std::make_shared<Entity>("MainCamera");
    
    auto transform = cameraEntity->addComponent<Transform>();
    transform->setPosition(glm::vec3(0.0f, 2.0f, -5.0f));
    transform->setRotation(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)));

    auto camera = cameraEntity->addComponent<Camera>();
    camera->setPerspective(45.0f, 1280.0f/720.0f, 0.1f, 1000.0f);
}

void DemoScene::setupModel() {
    auto& resourceManager = ResourceManager::getInstance();

    // Load shader
    auto shader = resourceManager.loadShader("default", 
        "assets/shaders/default.vert", 
        "assets/shaders/default.frag");

    // Create material
    auto material = resourceManager.createMaterial("default", shader);

    // Create model entity
    modelEntity = std::make_shared<Entity>("Model");
    
    auto transform = modelEntity->addComponent<Transform>();
    transform->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));

    auto meshRenderer = modelEntity->addComponent<MeshRenderer>();
    meshRenderer->setMaterial(material);

    // Load and set mesh (assuming we have a cube.obj)
    auto mesh = resourceManager.loadMesh("cube", "assets/models/cube.obj");
    if (mesh) {
        meshRenderer->setMesh(mesh);
    }
}

void DemoScene::update(float deltaTime) {
    Input::getInstance().update();
    handleInput(deltaTime);

    if (cameraEntity) cameraEntity->update(deltaTime);
    if (modelEntity) modelEntity->update(deltaTime);
}

void DemoScene::handleInput(float deltaTime) {
    auto& input = Input::getInstance();

    // Camera movement
    if (auto transform = cameraEntity->getComponent<Transform>()) {
        float moveSpeed = 5.0f * deltaTime;
        float rotateSpeed = 100.0f * deltaTime;

        // Translation
        if (input.isKeyPressed(GLFW_KEY_W))
            transform->translate(glm::vec3(0.0f, 0.0f, moveSpeed));
        if (input.isKeyPressed(GLFW_KEY_S))
            transform->translate(glm::vec3(0.0f, 0.0f, -moveSpeed));
        if (input.isKeyPressed(GLFW_KEY_A))
            transform->translate(glm::vec3(moveSpeed, 0.0f, 0.0f));
        if (input.isKeyPressed(GLFW_KEY_D))
            transform->translate(glm::vec3(-moveSpeed, 0.0f, 0.0f));

        // Rotation
        if (input.isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
            glm::vec2 mouseDelta = input.getMouseDelta() * rotateSpeed;
            transform->rotate(glm::vec3(-mouseDelta.y, -mouseDelta.x, 0.0f));
        }
    }

    // Model rotation
    if (auto transform = modelEntity->getComponent<Transform>()) {
        transform->rotate(glm::vec3(0.0f, 45.0f * deltaTime, 0.0f));
    }
}

void DemoScene::render() {
    engine.render();
}
