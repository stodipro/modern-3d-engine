#include "HealthSystem.hpp"

HealthSystem::HealthSystem()
    : maxHealth(100.0f)
    , currentHealth(100.0f)
    , damageMultiplier(1.0f)
    , regenAmount(0.0f)
    , invulnerable(false)
{}

void HealthSystem::setMaxHealth(float max) {
    maxHealth = max;
    if (currentHealth > maxHealth) {
        currentHealth = maxHealth;
    }
}

void HealthSystem::setHealth(float current) {
    currentHealth = glm::clamp(current, 0.0f, maxHealth);
}

void HealthSystem::takeDamage(float amount, Entity* damager) {
    if (invulnerable || amount <= 0 || !isAlive()) return;

    float actualDamage = amount * damageMultiplier;
    currentHealth = glm::max(0.0f, currentHealth - actualDamage);

    if (damageCallback) {
        damageCallback(actualDamage, damager);
    }

    if (currentHealth <= 0 && deathCallback) {
        deathCallback(damager);
    }
}

void HealthSystem::heal(float amount) {
    if (amount <= 0 || !isAlive()) return;

    float previousHealth = currentHealth;
    currentHealth = glm::min(maxHealth, currentHealth + amount);

    float actualHeal = currentHealth - previousHealth;
    if (actualHeal > 0 && healCallback) {
        healCallback(actualHeal);
    }
}

void HealthSystem::update(float deltaTime) {
    // Handle regeneration
    if (regenAmount > 0 && currentHealth < maxHealth && isAlive()) {
        heal(regenAmount * deltaTime);
    }
}
