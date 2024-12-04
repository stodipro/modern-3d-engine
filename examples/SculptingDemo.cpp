#include "SculptingDemo.hpp"
#include "../src/core/Input.hpp"
#include "../src/components/Transform.hpp"
#include "../src/components/MeshRenderer.hpp"
#include "../src/components/Light.hpp"
#include "../src/renderer/LightManager.hpp"

SculptingDemo::SculptingDemo() {}

SculptingDemo::~SculptingDemo() {}

bool SculptingDemo::initialize() {
    if (!engine.initialize()) {
        return false;
    }

    // Initialize systems
    Input::getInstance().initialize(engine.getWindow());
    SculptingSystem::getInstance().initialize();

    // Setup scene
    setupCamera();
    setupSculptMesh();
    setupLighting();

    return true;
}

void SculptingDemo::setupCamera() {
    cameraEntity = std::make_shared<Entity>("MainCamera");
    
    auto transform = cameraEntity->addComponent<Transform>();
    transform->setPosition(glm::vec3(0.0f, 2.0f, -5.0f));
    transform->setRotation(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)));

    auto camera = cameraEntity->addComponent<Camera>();
    camera->setPerspective(45.0f, 1280.0f/720.0f, 0.1f, 1000.0f);

    SculptingSystem::getInstance().setActiveCamera(camera);
}

void SculptingDemo::setupSculptMesh() {
    sculptMesh = std::make_shared<SculptMesh>();
    sculptMesh->initializeAsSphere(1.0f, 3); // Create a sphere with 3 subdivisions

    sculptMeshEntity = std::make_shared<Entity>("SculptMesh");
    auto transform = sculptMeshEntity->addComponent<Transform>();
    transform->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));

    auto meshRenderer = sculptMeshEntity->addComponent<MeshRenderer>();
    auto material = std::make_shared<Material>();
    // Set up material with appropriate shader and properties
    meshRenderer->setMaterial(material);
    meshRenderer->setMesh(sculptMesh->generateMesh());

    SculptingSystem::getInstance().setTargetMesh(sculptMesh);
}

void SculptingDemo::setupLighting() {
    // Create main directional light
    auto lightEntity = std::make_shared<Entity>("MainLight");
    auto lightTransform = lightEntity->addComponent<Transform>();
    lightTransform->setPosition(glm::vec3(5.0f, 5.0f, -5.0f));
    lightTransform->setRotation(glm::quat(glm::vec3(-45.0f, 45.0f, 0.0f)));

    auto light = lightEntity->addComponent<Light>();
    light->setType(Light::Type::Directional);
    light->setColor(glm::vec3(1.0f));
    light->setIntensity(1.0f);

    LightManager::getInstance().addLight(light);
}

void SculptingDemo::update(float deltaTime) {
    Input::getInstance().update();
    handleInput(deltaTime);
    updateUI();

    // Update systems
    SculptingSystem::getInstance().update(deltaTime);

    // Update entities
    if (cameraEntity) cameraEntity->update(deltaTime);
    if (sculptMeshEntity) {
        sculptMeshEntity->update(deltaTime);
        
        // Update mesh renderer with new mesh data when sculpted
        if (auto meshRenderer = sculptMeshEntity->getComponent<MeshRenderer>()) {
            meshRenderer->setMesh(sculptMesh->generateMesh());
        }
    }
}

void SculptingDemo::handleInput(float deltaTime) {
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

        // Rotation with right mouse button
        if (input.isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
            glm::vec2 mouseDelta = input.getMouseDelta() * rotateSpeed;
            transform->rotate(glm::vec3(-mouseDelta.y, -mouseDelta.x, 0.0f));
        }
    }

    // Tool selection
    if (input.isKeyJustPressed(GLFW_KEY_1))
        SculptingSystem::getInstance().setActiveTool(SculptingTool::Type::Pull);
    if (input.isKeyJustPressed(GLFW_KEY_2))
        SculptingSystem::getInstance().setActiveTool(SculptingTool::Type::Push);
    if (input.isKeyJustPressed(GLFW_KEY_3))
        SculptingSystem::getInstance().setActiveTool(SculptingTool::Type::Smooth);
    if (input.isKeyJustPressed(GLFW_KEY_4))
        SculptingSystem::getInstance().setActiveTool(SculptingTool::Type::Pinch);
}

void SculptingDemo::updateUI() {
    // In a real implementation, you'd want to add ImGui or similar for UI
    // For now, we'll just print controls to console
    static bool printedControls = false;
    if (!printedControls) {
        std::cout << "Sculpting Controls:\n";
        std::cout << "1-4: Switch tools (Pull, Push, Smooth, Pinch)\n";
        std::cout << "Left Mouse: Sculpt\n";
        std::cout << "Right Mouse: Rotate Camera\n";
        std::cout << "Mouse Wheel: Adjust Tool Size\n";
        std::cout << "WASD: Move Camera\n";
        printedControls = true;
    }
}

void SculptingDemo::render() {
    engine.render();
}
