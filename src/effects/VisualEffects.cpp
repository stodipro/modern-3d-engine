#include "VisualEffects.hpp"
#include "ParticleSystem.hpp"
#include "LineRenderer.hpp"
#include <algorithm>

VisualEffects::VisualEffects() {
    initializeBulletTrailRenderer();
}

VisualEffects::~VisualEffects() {}

void VisualEffects::update(float deltaTime) {
    updateActiveEffects(deltaTime);
    removeFinishedEffects();
}

void VisualEffects::createMuzzleFlash(const glm::vec3& position, const glm::vec3& direction) {
    auto particles = std::make_shared<ParticleSystem>();
    
    // Configure muzzle flash particles
    ParticleSystem::Settings settings;
    settings.startSize = 0.2f;
    settings.endSize = 0.05f;
    settings.startColor = glm::vec4(1.0f, 0.7f, 0.3f, 1.0f);
    settings.endColor = glm::vec4(1.0f, 0.4f, 0.1f, 0.0f);
    settings.velocity = direction * 2.0f;
    settings.spread = 30.0f;
    settings.lifetime = 0.1f;
    settings.emissionRate = 50;
    settings.emitCount = 10;
    
    particles->setSettings(settings);
    particles->setPosition(position);
    
    ActiveEffect effect;
    effect.particles = particles;
    effect.lifetime = 0.0f;
    effect.duration = 0.1f;
    
    activeEffects.push_back(effect);
}

void VisualEffects::createBulletTrail(const glm::vec3& start, const glm::vec3& end) {
    // Add trail to renderer
    bulletTrailRenderer->addLine(start, end, glm::vec4(1.0f, 1.0f, 0.5f, 0.5f), 0.2f);

    // Add subtle particle trail
    auto particles = std::make_shared<ParticleSystem>();
    
    ParticleSystem::Settings settings;
    settings.startSize = 0.05f;
    settings.endSize = 0.0f;
    settings.startColor = glm::vec4(1.0f, 1.0f, 0.5f, 0.3f);
    settings.endColor = glm::vec4(1.0f, 1.0f, 0.5f, 0.0f);
    settings.velocity = glm::vec3(0.0f);
    settings.lifetime = 0.5f;
    settings.emissionRate = 100;
    settings.emitCount = 20;
    
    particles->setSettings(settings);
    
    // Emit particles along the trail
    glm::vec3 trailDir = end - start;
    float trailLength = glm::length(trailDir);
    trailDir = glm::normalize(trailDir);
    
    for (float t = 0; t < trailLength; t += 0.1f) {
        particles->setPosition(start + trailDir * t);
        particles->emit();
    }
    
    ActiveEffect effect;
    effect.particles = particles;
    effect.lifetime = 0.0f;
    effect.duration = 0.5f;
    
    activeEffects.push_back(effect);
}

void VisualEffects::createImpactEffect(const glm::vec3& position, const glm::vec3& normal) {
    auto particles = std::make_shared<ParticleSystem>();
    
    ParticleSystem::Settings settings;
    settings.startSize = 0.1f;
    settings.endSize = 0.0f;
    settings.startColor = glm::vec4(1.0f, 0.7f, 0.3f, 1.0f);
    settings.endColor = glm::vec4(1.0f, 0.4f, 0.1f, 0.0f);
    settings.velocity = normal * 3.0f;
    settings.spread = 45.0f;
    settings.lifetime = 0.5f;
    settings.emissionRate = 100;
    settings.emitCount = 20;
    
    particles->setSettings(settings);
    particles->setPosition(position);
    
    ActiveEffect effect;
    effect.particles = particles;
    effect.lifetime = 0.0f;
    effect.duration = 0.5f;
    
    activeEffects.push_back(effect);
}

void VisualEffects::createBloodSplatter(const glm::vec3& position, const glm::vec3& direction) {
    auto particles = std::make_shared<ParticleSystem>();
    
    ParticleSystem::Settings settings;
    settings.startSize = 0.1f;
    settings.endSize = 0.05f;
    settings.startColor = glm::vec4(0.8f, 0.0f, 0.0f, 1.0f);
    settings.endColor = glm::vec4(0.5f, 0.0f, 0.0f, 0.0f);
    settings.velocity = direction * 5.0f;
    settings.spread = 30.0f;
    settings.lifetime = 1.0f;
    settings.emissionRate = 50;
    settings.emitCount = 30;
    
    particles->setSettings(settings);
    particles->setPosition(position);
    
    ActiveEffect effect;
    effect.particles = particles;
    effect.lifetime = 0.0f;
    effect.duration = 1.0f;
    
    activeEffects.push_back(effect);
}

void VisualEffects::initializeBulletTrailRenderer() {
    bulletTrailRenderer = std::make_shared<LineRenderer>();
    // Configure line renderer settings
    bulletTrailRenderer->setFadeTime(0.2f);
    bulletTrailRenderer->setMaxLines(100);
}

void VisualEffects::updateActiveEffects(float deltaTime) {
    for (auto& effect : activeEffects) {
        effect.lifetime += deltaTime;
        effect.particles->update(deltaTime);
    }
    
    bulletTrailRenderer->update(deltaTime);
}

void VisualEffects::removeFinishedEffects() {
    activeEffects.erase(
        std::remove_if(activeEffects.begin(), activeEffects.end(),
            [](const ActiveEffect& effect) {
                return effect.lifetime >= effect.duration;
            }),
        activeEffects.end());
}
