#pragma once
#include "../src/core/Engine.hpp"
#include "../src/gameplay/FPSController.hpp"
#include "../src/gameplay/WeaponSystem.hpp"
#include "../src/gameplay/HealthSystem.hpp"
#include "../src/gameplay/AIController.hpp"
#include "../src/effects/VisualEffects.hpp"
#include <memory>

class FPSDemo {
public:
    FPSDemo();
    ~FPSDemo();

    bool initialize();
    void update(float deltaTime);
    void render();

private:
    Engine engine;
    std::shared_ptr<Entity> playerEntity;
    std::vector<std::shared_ptr<Entity>> aiEntities;
    std::vector<glm::vec3> spawnPoints;

    void setupPlayer();
    void setupWeapons();
    void setupLevel();
    void setupAI();
    void handleInput(float deltaTime);
    void spawnAI(const glm::vec3& position);
    void respawnAI(Entity* deadAI);
    
    // Combat event handlers
    void onPlayerDamaged(float amount, Entity* attacker);
    void onAIDamaged(Entity* ai, float amount, Entity* attacker);
    void onAIKilled(Entity* ai, Entity* killer);
};
