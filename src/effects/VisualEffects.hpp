#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include "../components/Component.hpp"

class ParticleSystem;
class LineRenderer;

class VisualEffects : public Component {
public:
    VisualEffects();
    ~VisualEffects();

    void update(float deltaTime) override;

    // Weapon effects
    void createMuzzleFlash(const glm::vec3& position, const glm::vec3& direction);
    void createBulletTrail(const glm::vec3& start, const glm::vec3& end);
    void createImpactEffect(const glm::vec3& position, const glm::vec3& normal);
    void createShellCasing(const glm::vec3& position, const glm::vec3& direction);

    // Impact effects
    void createBloodSplatter(const glm::vec3& position, const glm::vec3& direction);
    void createSparkEffect(const glm::vec3& position, const glm::vec3& normal);
    void createDustEffect(const glm::vec3& position);

    // Environment effects
    void createSmokeTrail(const glm::vec3& position, float duration);
    void createExplosion(const glm::vec3& position, float radius);

private:
    struct ActiveEffect {
        std::shared_ptr<ParticleSystem> particles;
        float lifetime;
        float duration;
    };

    std::vector<ActiveEffect> activeEffects;
    std::shared_ptr<LineRenderer> bulletTrailRenderer;

    // Helper functions
    void initializeBulletTrailRenderer();
    void updateActiveEffects(float deltaTime);
    void removeFinishedEffects();
};
