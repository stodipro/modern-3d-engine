#include "GameHUD.hpp"
#include "../gameplay/WeaponSystem.hpp"
#include "../gameplay/HealthSystem.hpp"
#include <algorithm>

void GameHUD::initialize() {
    showCrosshairUI = true;
    showHealthUI = true;
    showAmmoUI = true;
    damageIndicatorTimer = 0.0f;
    hitMarkerTimer = 0.0f;
    playerHealth = nullptr;
    playerWeapons = nullptr;
}

void GameHUD::render() {
    auto& ui = UISystem::getInstance();
    
    // Game UI window (fullscreen, no decorations)
    ui.beginWindow("GameHUD", 0, 0, 1280, 720,
                  ImGuiWindowFlags_NoDecoration |
                  ImGuiWindowFlags_NoBackground |
                  ImGuiWindowFlags_NoInputs);

    if (showCrosshairUI) renderCrosshair();
    if (showHealthUI) renderHealthBar();
    if (showAmmoUI) renderAmmoCounter();
    
    renderDamageIndicator();
    renderHitMarker();
    renderMessages();
    renderKillFeed();

    ui.endWindow();
}

void GameHUD::renderCrosshair() {
    auto& ui = UISystem::getInstance();
    
    // Center of screen
    ImVec2 center = ImVec2(1280.0f/2, 720.0f/2);
    
    // Draw crosshair lines
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    float size = 10.0f;
    float thickness = 2.0f;
    ImU32 color = IM_COL32(255, 255, 255, 200);

    // Horizontal line
    draw_list->AddLine(
        ImVec2(center.x - size, center.y),
        ImVec2(center.x + size, center.y),
        color, thickness);

    // Vertical line
    draw_list->AddLine(
        ImVec2(center.x, center.y - size),
        ImVec2(center.x, center.y + size),
        color, thickness);
}

void GameHUD::renderHealthBar() {
    if (!playerHealth) return;

    auto& ui = UISystem::getInstance();
    float health = playerHealth->getHealth();
    float maxHealth = playerHealth->getMaxHealth();
    float healthPercent = health / maxHealth;

    // Health bar background
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 pos = ImVec2(50, 720.0f - 50);
    ImVec2 size = ImVec2(200, 20);
    
    // Background
    draw_list->AddRectFilled(
        pos,
        ImVec2(pos.x + size.x, pos.y + size.y),
        IM_COL32(60, 60, 60, 200));

    // Health bar
    ImU32 healthColor = IM_COL32(
        static_cast<int>(255 * (1 - healthPercent)),
        static_cast<int>(255 * healthPercent),
        0, 200);

    draw_list->AddRectFilled(
        pos,
        ImVec2(pos.x + size.x * healthPercent, pos.y + size.y),
        healthColor);

    // Health text
    std::string healthText = std::to_string(static_cast<int>(health)) + "/" +
                            std::to_string(static_cast<int>(maxHealth));
    
    draw_list->AddText(
        ImVec2(pos.x + size.x/2 - 20, pos.y),
        IM_COL32(255, 255, 255, 255),
        healthText.c_str());
}

void GameHUD::renderAmmoCounter() {
    if (!playerWeapons) return;

    auto& ui = UISystem::getInstance();
    
    // Ammo counter position (bottom right)
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 pos = ImVec2(1280.0f - 150, 720.0f - 50);

    // Current weapon info
    std::string ammoText = std::to_string(playerWeapons->getCurrentAmmo()) + "/" +
                          std::to_string(playerWeapons->getTotalAmmo());

    // Weapon name
    draw_list->AddText(
        ImVec2(pos.x, pos.y - 20),
        IM_COL32(255, 255, 255, 255),
        playerWeapons->getActiveWeapon().name.c_str());

    // Ammo count
    draw_list->AddText(
        pos,
        IM_COL32(255, 255, 255, 255),
        ammoText.c_str());
}

void GameHUD::renderDamageIndicator() {
    if (damageIndicatorTimer <= 0.0f) return;

    auto& ui = UISystem::getInstance();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 center = ImVec2(1280.0f/2, 720.0f/2);
    
    // Calculate indicator position
    float radius = 100.0f;
    ImVec2 indicatorPos = ImVec2(
        center.x + damageDirection.x * radius,
        center.y + damageDirection.z * radius
    );

    // Draw damage indicator
    float alpha = std::min(damageIndicatorTimer * 2, 1.0f);
    draw_list->AddCircle(
        indicatorPos,
        10.0f,
        IM_COL32(255, 0, 0, static_cast<int>(255 * alpha)),
        12, 2.0f);

    damageIndicatorTimer -= ImGui::GetIO().DeltaTime;
}

void GameHUD::renderHitMarker() {
    if (hitMarkerTimer <= 0.0f) return;

    auto& ui = UISystem::getInstance();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 center = ImVec2(1280.0f/2, 720.0f/2);
    
    float alpha = std::min(hitMarkerTimer * 2, 1.0f);
    ImU32 color = IM_COL32(255, 255, 255, static_cast<int>(255 * alpha));
    float size = 15.0f;

    // Draw hit marker (X)
    draw_list->AddLine(
        ImVec2(center.x - size, center.y - size),
        ImVec2(center.x + size, center.y + size),
        color, 2.0f);
    draw_list->AddLine(
        ImVec2(center.x - size, center.y + size),
        ImVec2(center.x + size, center.y - size),
        color, 2.0f);

    hitMarkerTimer -= ImGui::GetIO().DeltaTime;
}

void GameHUD::renderMessages() {
    auto& ui = UISystem::getInstance();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    float yPos = 100.0f;

    for (auto it = messages.begin(); it != messages.end();) {
        float alpha = std::min(it->timer / (it->duration * 0.5f), 1.0f);
        draw_list->AddText(
            ImVec2(50, yPos),
            IM_COL32(255, 255, 255, static_cast<int>(255 * alpha)),
            it->text.c_str());

        it->timer -= ImGui::GetIO().DeltaTime;
        if (it->timer <= 0.0f) {
            it = messages.erase(it);
        } else {
            ++it;
            yPos += 30.0f;
        }
    }
}

void GameHUD::renderKillFeed() {
    auto& ui = UISystem::getInstance();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    float yPos = 50.0f;

    for (auto it = killFeed.begin(); it != killFeed.end();) {
        float alpha = std::min(it->timer / (it->duration * 0.5f), 1.0f);
        draw_list->AddText(
            ImVec2(1280.0f - 300, yPos),
            IM_COL32(255, 255, 255, static_cast<int>(255 * alpha)),
            it->text.c_str());

        it->timer -= ImGui::GetIO().DeltaTime;
        if (it->timer <= 0.0f) {
            it = killFeed.erase(it);
        } else {
            ++it;
            yPos += 25.0f;
        }
    }
}

void GameHUD::showDamageIndicator(const glm::vec3& damageDir) {
    damageDirection = glm::normalize(damageDir);
    damageIndicatorTimer = 1.0f;
}

void GameHUD::showHitMarker() {
    hitMarkerTimer = 0.5f;
}

void GameHUD::showMessage(const std::string& message, float duration) {
    Message msg;
    msg.text = message;
    msg.timer = duration;
    msg.duration = duration;
    messages.push_back(msg);
}

void GameHUD::showKillFeed(const std::string& killer, const std::string& victim) {
    Message msg;
    msg.text = killer + " killed " + victim;
    msg.timer = 5.0f;
    msg.duration = 5.0f;
    killFeed.push_back(msg);
}
