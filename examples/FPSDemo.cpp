#include "FPSDemo.hpp"
#include "../src/core/Input.hpp"
#include "../src/components/Transform.hpp"
#include "../src/components/Camera.hpp"
#include "../src/components/MeshRenderer.hpp"
#include "../src/physics/RigidBody.hpp"
#include "../src/physics/CapsuleCollider.hpp"
#include "../src/physics/BoxCollider.hpp"

FPSDemo::FPSDemo() {}

FPSDemo::~FPSDemo() {}

bool FPSDemo::initialize() {
    if (!engine.initialize()) {
        return false;
    }

    // Initialize systems
    Input::getInstance().initialize(engine.getWindow());

    // Setup scene
    setupPlayer();
    setupWeapons();
    setupLevel();
    setupTargets();

    // Lock cursor for FPS controls
    glfwSetInputMode(engine.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return true;
}

void FPSDemo::setupPlayer() {
    playerEntity = std::make_shared<Entity>("Player");

    // Transform
    auto transform = playerEntity->addComponent<Transform>();
    transform->setPosition(glm::vec3(0.0f, 2.0f, 0.0f));

    // Camera
    auto camera = playerEntity->addComponent<Camera>();
    camera->setPerspective(90.0f, 1280.0f/720.0f, 0.1f, 1000.0f);

    // FPS Controller
    auto controller = playerEntity->addComponent<FPSController>();
    controller->setMoveSpeed(5.0f);
    controller->setSprintMultiplier(1.5f);
    controller->setJumpForce(5.0f);
    controller->setMouseSensitivity(0.2f);

    // Physics
    auto rb = playerEntity->addComponent<RigidBody>();
    rb->setMass(70.0f); // 70 kg
    rb->setDrag(0.1f);

    auto collider = playerEntity->addComponent<CapsuleCollider>();
    collider->setRadius(0.3f);
    collider->setHeight(1.8f);

    // Health System
    auto health = playerEntity->addComponent<HealthSystem>();
    health->setMaxHealth(100.0f);
    health->setRegeneration(1.0f); // 1 HP per second

    // Weapon System
    auto weapons = playerEntity->addComponent<WeaponSystem>();

    // Setup some default weapons
    WeaponData rifle;
    rifle.name = "Assault Rifle";
    rifle.damage = 25.0f;
    rifle.fireRate = 10.0f;
    rifle.reloadTime = 2.0f;
    rifle.magazineSize = 30;
    rifle.range = 100.0f;
    rifle.spread = 2.0f;
    rifle.automatic = true;
    rifle.recoilVertical = 0.1f;
    rifle.recoilHorizontal = 0.05f;
    rifle.recoilRecovery = 5.0f;

    WeaponData pistol;
    pistol.name = "Pistol";
    pistol.damage = 15.0f;
    pistol.fireRate = 5.0f;
    pistol.reloadTime = 1.5f;
    pistol.magazineSize = 12;
    pistol.range = 50.0f;
    pistol.spread = 1.0f;
    pistol.automatic = false;
    pistol.recoilVertical = 0.15f;
    pistol.recoilHorizontal = 0.02f;
    pistol.recoilRecovery = 8.0f;

    weapons->addWeapon(rifle);
    weapons->addWeapon(pistol);
}

void FPSDemo::setupLevel() {
    // Create ground
    auto ground = std::make_shared<Entity>("Ground");
    auto groundTransform = ground->addComponent<Transform>();
    groundTransform->setPosition(glm::vec3(0.0f, -0.5f, 0.0f));
    groundTransform->setScale(glm::vec3(50.0f, 1.0f, 50.0f));

    auto groundCollider = ground->addComponent<BoxCollider>();
    groundCollider->setSize(glm::vec3(50.0f, 1.0f, 50.0f));

    // Add some walls and obstacles
    createWall(glm::vec3(10.0f, 2.0f, 0.0f), glm::vec3(1.0f, 4.0f, 20.0f));
    createWall(glm::vec3(-10.0f, 2.0f, 0.0f), glm::vec3(1.0f, 4.0f, 20.0f));
    createWall(glm::vec3(0.0f, 2.0f, 10.0f), glm::vec3(20.0f, 4.0f, 1.0f));
    createWall(glm::vec3(0.0f, 2.0f, -10.0f), glm::vec3(20.0f, 4.0f, 1.0f));

    // Add some cover objects
    createCover(glm::vec3(5.0f, 1.0f, 5.0f));
    createCover(glm::vec3(-5.0f, 1.0f, -5.0f));
    createCover(glm::vec3(5.0f, 1.0f, -5.0f));
    createCover(glm::vec3(-5.0f, 1.0f, 5.0f));
}

void FPSDemo::createWall(const glm::vec3& position, const glm::vec3& size) {
    auto wall = std::make_shared<Entity>("Wall");
    auto transform = wall->addComponent<Transform>();
    transform->setPosition(position);
    transform->setScale(size);

    auto collider = wall->addComponent<BoxCollider>();
    collider->setSize(size);
}

void FPSDemo::createCover(const glm::vec3& position) {
    auto cover = std::make_shared<Entity>("Cover");
    auto transform = cover->addComponent<Transform>();
    transform->setPosition(position);
    transform->setScale(glm::vec3(2.0f, 2.0f, 2.0f));

    auto collider = cover->addComponent<BoxCollider>();
    collider->setSize(glm::vec3(2.0f));
}

void FPSDemo::setupTargets() {
    // Create some target practice dummies
    spawnTarget(glm::vec3(8.0f, 1.0f, 8.0f));
    spawnTarget(glm::vec3(-8.0f, 1.0f, -8.0f));
    spawnTarget(glm::vec3(8.0f, 1.0f, -8.0f));
    spawnTarget(glm::vec3(-8.0f, 1.0f, 8.0f));
}

void FPSDemo::spawnTarget(const glm::vec3& position) {
    auto target = std::make_shared<Entity>("Target");
    
    auto transform = target->addComponent<Transform>();
    transform->setPosition(position);

    auto collider = target->addComponent<CapsuleCollider>();
    collider->setRadius(0.3f);
    collider->setHeight(1.8f);

    auto health = target->addComponent<HealthSystem>();
    health->setMaxHealth(100.0f);
    health->onDeath([this, target](Entity* killer) {
        // Remove target from list when killed
        auto it = std::find(targets.begin(), targets.end(), target);
        if (it != targets.end()) {
            targets.erase(it);
        }
    });

    targets.push_back(target);
}

void FPSDemo::update(float deltaTime) {
    handleInput(deltaTime);

    // Update player
    if (playerEntity) {
        playerEntity->update(deltaTime);
    }

    // Update targets
    for (auto& target : targets) {
        target->update(deltaTime);
    }
}

void FPSDemo::handleInput(float deltaTime) {
    auto& input = Input::getInstance();

    // Weapon controls
    if (auto weapons = playerEntity->getComponent<WeaponSystem>()) {
        if (input.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
            weapons->startFiring();
        } else {
            weapons->stopFiring();
        }

        if (input.isKeyJustPressed(GLFW_KEY_R)) {
            weapons->reload();
        }

        if (input.isKeyJustPressed(GLFW_KEY_1) || input.isKeyJustPressed(GLFW_KEY_2)) {
            weapons->switchWeapon(input.isKeyJustPressed(GLFW_KEY_1) ? 0 : 1);
        }
    }

    // Toggle cursor lock with Escape
    if (input.isKeyJustPressed(GLFW_KEY_ESCAPE)) {
        bool locked = glfwGetInputMode(engine.getWindow(), GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
        glfwSetInputMode(engine.getWindow(), GLFW_CURSOR, locked ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }
}

void FPSDemo::render() {
    engine.render();
}
