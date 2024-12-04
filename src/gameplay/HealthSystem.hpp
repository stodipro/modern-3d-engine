#pragma once
#include "../components/Component.hpp"
#include <functional>

class HealthSystem : public Component {
public:
    HealthSystem();

    void setMaxHealth(float max);
    void setHealth(float current);
    void takeDamage(float amount, Entity* damager = nullptr);
    void heal(float amount);

    float getHealth() const { return currentHealth; }
    float getMaxHealth() const { return maxHealth; }
    bool isAlive() const { return currentHealth > 0; }

    // Event callbacks
    void onDamage(const std::function<void(float, Entity*)>& callback) { damageCallback = callback; }
    void onHeal(const std::function<void(float)>& callback) { healCallback = callback; }
    void onDeath(const std::function<void(Entity*)>& callback) { deathCallback = callback; }

    // Damage modifiers
    void setDamageMultiplier(float multiplier) { damageMultiplier = multiplier; }
    void setInvulnerable(bool invuln) { invulnerable = invuln; }
    void setRegeneration(float amountPerSecond) { regenAmount = amountPerSecond; }

    void update(float deltaTime) override;

private:
    float maxHealth;
    float currentHealth;
    float damageMultiplier;
    float regenAmount;
    bool invulnerable;

    std::function<void(float, Entity*)> damageCallback;
    std::function<void(float)> healCallback;
    std::function<void(Entity*)> deathCallback;
};
