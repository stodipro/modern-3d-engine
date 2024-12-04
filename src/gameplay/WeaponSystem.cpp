#include "WeaponSystem.hpp"
#include "../core/Input.hpp"
#include "../components/Transform.hpp"
#include "../physics/PhysicsSystem.hpp"
#include <GLFW/glfw3.h>

WeaponSystem::WeaponSystem()
    : currentWeapon(0)
    , firing(false)
    , reloading(false)
    , fireTimer(0.0f)
    , reloadTimer(0.0f)
    , currentAmmo(0)
    , totalAmmo(0)
    , currentRecoil(0.0f)
    , recoilVelocity(0.0f)
{}

void WeaponSystem::update(float deltaTime) {
    if (weapons.empty()) return;

    // Update timers
    if (fireTimer > 0.0f) {
        fireTimer -= deltaTime;
    }

    if (reloading) {
        reloadTimer -= deltaTime;
        if (reloadTimer <= 0.0f) {
            // Finish reloading
            reloading = false;
            const auto& weapon = weapons[currentWeapon];
            int ammoNeeded = weapon.magazineSize - currentAmmo;
            int ammoAvailable = std::min(ammoNeeded, totalAmmo);
            currentAmmo += ammoAvailable;
            totalAmmo -= ammoAvailable;
        }
    }

    // Handle firing
    handleFiring(deltaTime);

    // Handle recoil recovery
    handleRecoil(deltaTime);
}

void WeaponSystem::addWeapon(const WeaponData& data) {
    weapons.push_back(data);
    if (weapons.size() == 1) {
        // First weapon added, initialize ammo
        currentAmmo = data.magazineSize;
        totalAmmo = data.magazineSize * 3;
    }
}

void WeaponSystem::switchWeapon(int index) {
    if (index >= 0 && index < weapons.size() && index != currentWeapon) {
        currentWeapon = index;
        firing = false;
        reloading = false;
        fireTimer = 0.0f;
        currentRecoil = glm::vec2(0.0f);
    }
}

void WeaponSystem::nextWeapon() {
    switchWeapon((currentWeapon + 1) % weapons.size());
}

void WeaponSystem::previousWeapon() {
    switchWeapon((currentWeapon - 1 + weapons.size()) % weapons.size());
}

void WeaponSystem::startFiring() {
    firing = true;
    if (canFire()) {
        fire();
    }
}

void WeaponSystem::stopFiring() {
    firing = false;
}

void WeaponSystem::reload() {
    if (!reloading && currentAmmo < weapons[currentWeapon].magazineSize && totalAmmo > 0) {
        reloading = true;
        reloadTimer = weapons[currentWeapon].reloadTime;
        firing = false;
    }
}

void WeaponSystem::handleFiring(float deltaTime) {
    if (firing && canFire()) {
        fire();
    }
}

void WeaponSystem::handleRecoil(float deltaTime) {
    const auto& weapon = weapons[currentWeapon];
    float recovery = weapon.recoilRecovery * deltaTime;

    // Apply spring-like recovery
    glm::vec2 recoilForce = -currentRecoil * recovery;
    recoilVelocity += recoilForce * deltaTime;
    recoilVelocity *= 0.9f; // Damping
    
    currentRecoil += recoilVelocity;

    // Apply recoil to camera
    if (auto transform = getEntity()->getComponent<Transform>()) {
        glm::quat rotation = transform->getRotation();
        rotation = glm::rotate(rotation, currentRecoil.y, glm::vec3(1, 0, 0));
        rotation = glm::rotate(rotation, currentRecoil.x, glm::vec3(0, 1, 0));
        transform->setRotation(rotation);
    }
}

void WeaponSystem::fire() {
    const auto& weapon = weapons[currentWeapon];
    
    // Consume ammo
    currentAmmo--;
    fireTimer = 1.0f / weapon.fireRate;

    // Calculate spread
    float spreadAngle = weapon.spread * (3.14159f / 180.0f);
    float randomAngle = static_cast<float>(rand()) / RAND_MAX * 2.0f * 3.14159f;
    float randomRadius = static_cast<float>(rand()) / RAND_MAX * spreadAngle;

    glm::vec3 spreadDir(
        cos(randomAngle) * sin(randomRadius),
        sin(randomAngle) * sin(randomRadius),
        cos(randomRadius)
    );

    // Transform spread direction to world space
    auto transform = getEntity()->getComponent<Transform>();
    glm::mat4 rotationMatrix = glm::mat4_cast(transform->getRotation());
    spreadDir = glm::vec3(rotationMatrix * glm::vec4(spreadDir, 0.0f));

    // Perform raycast
    PhysicsSystem::RaycastHit hit;
    glm::vec3 start = transform->getPosition();
    if (PhysicsSystem::getInstance().raycast(start, spreadDir, hit, weapon.range)) {
        // Handle hit
        // TODO: Apply damage to hit entity
    }

    // Apply recoil
    applyRecoil();
}

void WeaponSystem::applyRecoil() {
    const auto& weapon = weapons[currentWeapon];
    
    // Calculate random recoil
    float horizontalRecoil = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * weapon.recoilHorizontal;
    float verticalRecoil = weapon.recoilVertical;

    // Add recoil impulse
    recoilVelocity += glm::vec2(horizontalRecoil, verticalRecoil) * 0.1f;
}

bool WeaponSystem::canFire() const {
    return !weapons.empty() &&
           !reloading &&
           fireTimer <= 0.0f &&
           currentAmmo > 0 &&
           (weapons[currentWeapon].automatic || !firing);
}
