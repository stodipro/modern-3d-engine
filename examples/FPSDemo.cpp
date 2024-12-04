#include "FPSDemo.hpp"
#include "../src/core/Input.hpp"
#include "../src/components/Transform.hpp"
#include "../src/components/Camera.hpp"
#include "../src/physics/RigidBody.hpp"
#include "../src/physics/CapsuleCollider.hpp"

void FPSDemo::setupPlayer() {
    // ... (previous setup code) ...

    // Health System
    auto health = playerEntity->addComponent<HealthSystem>();
    health->setMaxHealth(100.0f);
    health->setRegeneration(1.0f);
    health->onDamage([this](float amount, Entity* attacker) {
        onPlayerDamaged(amount, attacker);
    });

    // Weapon System
    auto weapons = playerEntity->addComponent<WeaponSystem>();
    setupWeapons(weapons);

    // Visual Effects
    playerEntity->addComponent<VisualEffects>();
}

void FPSDemo::setupWeapons(WeaponSystem* weapons) {
    // Assault Rifle
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
    weapons->addWeapon(rifle);

    // Shotgun
    WeaponData shotgun;
    shotgun.name = "Shotgun";
    shotgun.damage = 15.0f;
    shotgun.fireRate = 1.0f;
    shotgun.reloadTime = 2.5f;
    shotgun.magazineSize = 6;
    shotgun.range = 20.0f;
    shotgun.spread = 10.0f;
    shotgun.automatic = false;
    shotgun.recoilVertical = 0.3f;
    shotgun.recoilHorizontal = 0.1f;
    shotgun.recoilRecovery = 3.0f;
    weapons->addWeapon(shotgun);
}

void FPSDemo::setupAI() {
    // Define spawn points around the level
    spawnPoints = {
        glm::vec3(10.0f, 0.0f, 10.0f),
        glm::vec3(-10.0f, 0.0f, -10.0f),
        glm::vec3(10.0f, 0.0f, -10.0f),
        glm::vec3(-10.0f, 0.0f, 10.0f)
    };

    // Spawn initial AI
    for (const auto& spawnPoint : spawnPoints) {
        spawnAI(spawnPoint);
    }
}

void FPSDemo::spawnAI(const glm::vec3& position) {
    auto aiEntity = std::make_shared<Entity>("AI");

    // Transform
    auto transform = aiEntity->addComponent<Transform>();
    transform->setPosition(position);

    // Physics
    auto rb = aiEntity->addComponent<RigidBody>();
    rb->setMass(70.0f);
    auto collider = aiEntity->addComponent<CapsuleCollider>();
    collider->setRadius(0.3f);
    collider->setHeight(1.8f);

    // AI Controller
    auto ai = aiEntity->addComponent<AIController>();
    ai->setMoveSpeed(3.0f);
    ai->setAggressionRange(15.0f);
    ai->setAttackRange(10.0f);
    ai->setAccuracy(0.7f);

    // Set patrol points
    std::vector<glm::vec3> patrolPoints;
    for (const auto& point : spawnPoints) {
        patrolPoints.push_back(point);
    }
    ai->setPatrolPoints(patrolPoints);

    // Health System
    auto health = aiEntity->addComponent<HealthSystem>();
    health->setMaxHealth(100.0f);
    health->onDamage([this, aiEntity](float amount, Entity* attacker) {
        onAIDamaged(aiEntity.get(), amount, attacker);
    });
    health->onDeath([this, aiEntity](Entity* killer) {
        onAIKilled(aiEntity.get(), killer);
    });

    // Weapon System
    auto weapons = aiEntity->addComponent<WeaponSystem>();
    WeaponData aiGun;
    aiGun.name = "AI Rifle";
    aiGun.damage = 10.0f;
    aiGun.fireRate = 5.0f;
    aiGun.magazineSize = 30;
    aiGun.range = 50.0f;
    aiGun.spread = 5.0f;
    aiGun.automatic = true;
    weapons->addWeapon(aiGun);

    // Visual Effects
    aiEntity->addComponent<VisualEffects>();

    aiEntities.push_back(aiEntity);
}

void FPSDemo::onPlayerDamaged(float amount, Entity* attacker) {
    // Create blood effect
    if (auto effects = playerEntity->getComponent<VisualEffects>()) {
        glm::vec3 damageDir = glm::normalize(playerEntity->getComponent<Transform>()->getPosition() -
                                            attacker->getComponent<Transform>()->getPosition());
        effects->createBloodSplatter(playerEntity->getComponent<Transform>()->getPosition(), damageDir);
    }
}

void FPSDemo::onAIDamaged(Entity* ai, float amount, Entity* attacker) {
    // Create blood effect
    if (auto effects = ai->getComponent<VisualEffects>()) {
        glm::vec3 damageDir = glm::normalize(ai->getComponent<Transform>()->getPosition() -
                                            attacker->getComponent<Transform>()->getPosition());
        effects->createBloodSplatter(ai->getComponent<Transform>()->getPosition(), damageDir);
    }
}

void FPSDemo::onAIKilled(Entity* ai, Entity* killer) {
    // Create death effects
    if (auto effects = ai->getComponent<VisualEffects>()) {
        effects->createBloodSplatter(ai->getComponent<Transform>()->getPosition(), glm::vec3(0, 1, 0));
    }

    // Schedule respawn
    respawnAI(ai);
}

void FPSDemo::respawnAI(Entity* deadAI) {
    // Find random spawn point away from player
    glm::vec3 playerPos = playerEntity->getComponent<Transform>()->getPosition();
    float maxDist = 0.0f;
    glm::vec3 bestSpawn = spawnPoints[0];

    for (const auto& point : spawnPoints) {
        float dist = glm::length(point - playerPos);
        if (dist > maxDist) {
            maxDist = dist;
            bestSpawn = point;
        }
    }

    // Respawn AI
    deadAI->getComponent<Transform>()->setPosition(bestSpawn);
    deadAI->getComponent<HealthSystem>()->setHealth(100.0f);
    deadAI->getComponent<AIController>()->setState(AIController::State::Patrol);
}

void FPSDemo::update(float deltaTime) {
    handleInput(deltaTime);

    // Update player
    playerEntity->update(deltaTime);

    // Update AI
    for (auto& ai : aiEntities) {
        ai->update(deltaTime);
    }
}

void FPSDemo::render() {
    engine.render();
}
