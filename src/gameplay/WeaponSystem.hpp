#pragma once
#include "../components/Component.hpp"
#include <string>
#include <vector>
#include <memory>

struct WeaponData {
    std::string name;
    float damage;
    float fireRate;          // Rounds per second
    float reloadTime;
    int magazineSize;
    float range;
    float spread;
    bool automatic;
    
    // Recoil parameters
    float recoilVertical;
    float recoilHorizontal;
    float recoilRecovery;
    
    // Animation parameters
    std::string idleAnim;
    std::string fireAnim;
    std::string reloadAnim;
};

class WeaponSystem : public Component {
public:
    WeaponSystem();
    void update(float deltaTime) override;

    // Weapon management
    void addWeapon(const WeaponData& data);
    void switchWeapon(int index);
    void nextWeapon();
    void previousWeapon();

    // Weapon actions
    void startFiring();
    void stopFiring();
    void reload();

    // State getters
    bool isFiring() const { return firing; }
    bool isReloading() const { return reloading; }
    int getCurrentAmmo() const { return currentAmmo; }
    int getTotalAmmo() const { return totalAmmo; }

private:
    std::vector<WeaponData> weapons;
    int currentWeapon;
    
    // Weapon state
    bool firing;
    bool reloading;
    float fireTimer;
    float reloadTimer;
    int currentAmmo;
    int totalAmmo;

    // Recoil state
    glm::vec2 currentRecoil;
    glm::vec2 recoilVelocity;

    // Internal methods
    void handleFiring(float deltaTime);
    void handleRecoil(float deltaTime);
    void fire();
    void applyRecoil();
    bool canFire() const;
};
