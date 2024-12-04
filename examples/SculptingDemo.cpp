#include "SculptingDemo.hpp"
#include "../src/core/Input.hpp"
#include "../src/components/Transform.hpp"
#include "../src/components/MeshRenderer.hpp"
#include "../src/components/Light.hpp"
#include "../src/renderer/LightManager.hpp"
#include "../src/ui/UISystem.hpp"
#include "../src/ui/SculptingUI.hpp"

SculptingDemo::SculptingDemo() {}

SculptingDemo::~SculptingDemo() {}

bool SculptingDemo::initialize() {
    if (!engine.initialize()) {
        return false;
    }

    // Initialize systems
    Input::getInstance().initialize(engine.getWindow());
    UISystem::getInstance().initialize(engine.getWindow());
    SculptingSystem::getInstance().initialize();
    SculptingUI::getInstance().initialize();

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
    sculptMesh->initializeAsSphere(1.0f, 3);

    sculptMeshEntity = std::make_shared<Entity>("SculptMesh");
    auto transform = sculptMeshEntity->addComponent<Transform>();
    transform->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));

    // Create and set up material
    auto material = std::make_shared<Material>();
    material->setShader(ResourceManager::getInstance().loadShader(
        "lit", "assets/shaders/lit.vert", "assets/shaders/lit.frag"));
    
    // Set default material properties
    material->setVector3("material.albedo", glm::vec3(0.7f, 0.7f, 0.7f));
    material->setFloat("material.metallic", 0.0f);
    material->setFloat("material.roughness", 0.5f);
    material->setFloat("material.ao", 1.0f);

    auto meshRenderer = sculptMeshEntity->addComponent<MeshRenderer>();
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

    // Add fill light
    auto fillLightEntity = std::make_shared<Entity>("FillLight");
    auto fillTransform = fillLightEntity->addComponent<Transform>();
    fillTransform->setPosition(glm::vec3(-3.0f, 2.0f, 2.0f));

    auto fillLight = fillLightEntity->addComponent<Light>();
    fillLight->setType(Light::Type::Point);
    fillLight->setColor(glm::vec3(0.6f, 0.8f, 1.0f));
    fillLight->setIntensity(0.5f);
    fillLight->setRange(10.0f);

    LightManager::getInstance().addLight(light);
    LightManager::getInstance().addLight(fillLight);
}

void SculptingDemo::update(float deltaTime) {
    Input::getInstance().update();
    handleInput(deltaTime);

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

    // Toggle UI with Tab
    if (input.isKeyJustPressed(GLFW_KEY_TAB)) {
        SculptingUI::getInstance().toggleVisible();
    }

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
}

void SculptingDemo::render() {
    // Begin frame
    UISystem::getInstance().beginFrame();

    // Render scene
    engine.render();

    // Render UI
    SculptingUI::getInstance().render();

    // End frame
    UISystem::getInstance().endFrame();
}
