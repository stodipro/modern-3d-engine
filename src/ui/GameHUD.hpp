#pragma once
#include "UISystem.hpp"
#include <string>
#include <memory>

class WeaponSystem;
class HealthSystem;

class GameHUD {
public:
    static GameHUD& getInstance() {
        static GameHUD instance;
        return instance;
    }

    void initialize();
    void render();

    // HUD elements visibility
    void showCrosshair(bool show) { showCrosshairUI = show; }
    void showHealthBar(bool show) { showHealthUI = show; }
    void showAmmo(bool show) { showAmmoUI = show; }
    void showDamageIndicator(const glm::vec3& damageDir);
    void showHitMarker();

    // Game state messages
    void showMessage(const std::string& message, float duration = 3.0f);
    void showKillFeed(const std::string& killer, const std::string& victim);

    // Link to game systems
    void setPlayerHealth(HealthSystem* health) { playerHealth = health; }
    void setPlayerWeapons(WeaponSystem* weapons) { playerWeapons = weapons; }

private:
    GameHUD() = default;
    ~GameHUD() = default;
    GameHUD(const GameHUD&) = delete;
    GameHUD& operator=(const GameHUD&) = delete;

    // System references
    HealthSystem* playerHealth;
    WeaponSystem* playerWeapons;

    // UI state
    bool showCrosshairUI;
    bool showHealthUI;
    bool showAmmoUI;
    float damageIndicatorTimer;
    float hitMarkerTimer;
    glm::vec3 damageDirection;

    // Message system
    struct Message {
        std::string text;
        float timer;
        float duration;
    };
    std::vector<Message> messages;
    std::vector<Message> killFeed;

    // Render components
    void renderCrosshair();
    void renderHealthBar();
    void renderAmmoCounter();
    void renderDamageIndicator();
    void renderHitMarker();
    void renderMessages();
    void renderKillFeed();
};
