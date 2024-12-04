#include "PhysicsDemo.hpp"
#include "../src/core/Input.hpp"
#include "../src/components/Transform.hpp"
#include "../src/components/Camera.hpp"
#include "../src/components/MeshRenderer.hpp"
#include "../src/components/Light.hpp"
#include "../src/physics/RigidBody.hpp"
#include "../src/physics/Collider.hpp"
#include "../src/renderer/LightManager.hpp"

PhysicsDemo::PhysicsDemo() {}

PhysicsDemo::~PhysicsDemo() {}

bool PhysicsDemo::initialize() {
    if (!engine.initialize()) {
        return false;
    }

    // Initialize systems
    Input::getInstance().initialize(engine.getWindow());
    PhysicsSystem::getInstance().initialize();
    
    // Setup scene
    setupCamera();
    setupScene();

    return true;
}

void PhysicsDemo::setupCamera() {
    cameraEntity = std::make_shared<Entity>("MainCamera");
    
    auto transform = cameraEntity->addComponent<Transform>();
    transform->setPosition(glm::vec3(0.0f, 5.0f, -10.0f));
    transform->setRotation(glm::quat(glm::vec3(-0.3f, 0.0f, 0.0f)));

    auto camera = cameraEntity->addComponent<Camera>();
    camera->setPerspective(45.0f, 1280.0f/720.0f, 0.1f, 1000.0f);
}

void PhysicsDemo::setupScene() {
    // Create ground plane
    auto ground = std::make_shared<Entity>("Ground");
    auto groundTransform = ground->addComponent<Transform>();
    groundTransform->setPosition(glm::vec3(0.0f, -1.0f, 0.0f));
    groundTransform->setScale(glm::vec3(20.0f, 0.2f, 20.0f));

    auto groundCollider = ground->addComponent<BoxCollider>();
    groundCollider->setSize(glm::vec3(20.0f, 0.2f, 20.0f));

    auto groundRb = ground->addComponent<RigidBody>();
    groundRb->setKinematic(true); // Ground doesn't move

    // Add ground to physics objects
    physicsObjects.push_back(ground);

    // Create some dynamic objects
    createPhysicsObject(glm::vec3(0.0f, 5.0f, 0.0f));
    createPhysicsObject(glm::vec3(1.0f, 7.0f, 0.0f));
    createPhysicsObject(glm::vec3(-1.0f, 9.0f, 0.0f));

    // Setup lighting
    auto light = std::make_shared<Entity>("MainLight");
    auto lightTransform = light->addComponent<Transform>();
    lightTransform->setPosition(glm::vec3(5.0f, 10.0f, -5.0f));

    auto directionalLight = light->addComponent<Light>();
    directionalLight->setType(Light::Type::Directional);
    directionalLight->setColor(glm::vec3(1.0f));
    directionalLight->setIntensity(1.0f);

    LightManager::getInstance().addLight(directionalLight);
}

void PhysicsDemo::createPhysicsObject(const glm::vec3& position, bool isKinematic) {
    auto entity = std::make_shared<Entity>("PhysicsObject");
    
    // Transform
    auto transform = entity->addComponent<Transform>();
    transform->setPosition(position);

    // Collider
    auto collider = entity->addComponent<SphereCollider>();
    collider->setRadius(0.5f);
    collider->setMaterial(0.7f, 0.3f); // Bouncy material

    // RigidBody
    auto rb = entity->addComponent<RigidBody>();
    rb->setMass(1.0f);
    rb->setDrag(0.01f);
    rb->setKinematic(isKinematic);

    // Visual representation
    auto meshRenderer = entity->addComponent<MeshRenderer>();
    // Setup mesh and material for visualization

    physicsObjects.push_back(entity);
}

void PhysicsDemo::update(float deltaTime) {
    Input::getInstance().update();
    handleInput(deltaTime);

    // Update all entities
    for (auto& entity : physicsObjects) {
        entity->update(deltaTime);
    }
    if (cameraEntity) cameraEntity->update(deltaTime);

    // Physics system updates automatically
}

void PhysicsDemo::handleInput(float deltaTime) {
    auto& input = Input::getInstance();

    // Camera movement
    if (auto transform = cameraEntity->getComponent<Transform>()) {
        float moveSpeed = 5.0f * deltaTime;
        float rotateSpeed = 100.0f * deltaTime;

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

    // Spawn new physics object on left click
    if (input.isMouseButtonJustPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        if (auto camera = cameraEntity->getComponent<Camera>()) {
            auto transform = cameraEntity->getComponent<Transform>();
            glm::vec3 spawnPos = transform->getPosition() + transform->getForward() * 2.0f;
            createPhysicsObject(spawnPos);

            // Apply initial velocity in camera's forward direction
            auto& objects = physicsObjects;
            if (auto rb = objects.back()->getComponent<RigidBody>()) {
                rb->addImpulse(transform->getForward() * 10.0f);
            }
        }
    }
}

void PhysicsDemo::render() {
    engine.render();
}
