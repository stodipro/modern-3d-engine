#include "LightManager.hpp"
#include "Shader.hpp"
#include <algorithm>

void LightManager::addLight(Light* light) {
    if (!light) return;

    switch (light->getType()) {
        case Light::Type::Directional:
            if (directionalLights.size() < MAX_DIRECTIONAL_LIGHTS) {
                directionalLights.push_back(light);
            }
            break;
        case Light::Type::Point:
            if (pointLights.size() < MAX_POINT_LIGHTS) {
                pointLights.push_back(light);
            }
            break;
        case Light::Type::Spot:
            if (spotLights.size() < MAX_SPOT_LIGHTS) {
                spotLights.push_back(light);
            }
            break;
    }
}

void LightManager::removeLight(Light* light) {
    if (!light) return;

    auto removeFromVector = [light](std::vector<Light*>& lights) {
        auto it = std::find(lights.begin(), lights.end(), light);
        if (it != lights.end()) {
            lights.erase(it);
        }
    };

    switch (light->getType()) {
        case Light::Type::Directional:
            removeFromVector(directionalLights);
            break;
        case Light::Type::Point:
            removeFromVector(pointLights);
            break;
        case Light::Type::Spot:
            removeFromVector(spotLights);
            break;
    }
}

void LightManager::clear() {
    directionalLights.clear();
    pointLights.clear();
    spotLights.clear();
}

void LightManager::applyLights(Shader* shader) {
    if (!shader) return;

    applyDirectionalLights(shader);
    applyPointLights(shader);
    applySpotLights(shader);
}

void LightManager::applyDirectionalLights(Shader* shader) {
    shader->setInt("numDirectionalLights", static_cast<int>(directionalLights.size()));

    for (size_t i = 0; i < directionalLights.size() && i < MAX_DIRECTIONAL_LIGHTS; ++i) {
        std::string base = "directionalLights[" + std::to_string(i) + "].";
        shader->setVec3(base + "direction", directionalLights[i]->getDirection());
        shader->setVec3(base + "color", directionalLights[i]->getColor());
        shader->setFloat(base + "intensity", directionalLights[i]->getIntensity());
    }
}

void LightManager::applyPointLights(Shader* shader) {
    shader->setInt("numPointLights", static_cast<int>(pointLights.size()));

    for (size_t i = 0; i < pointLights.size() && i < MAX_POINT_LIGHTS; ++i) {
        std::string base = "pointLights[" + std::to_string(i) + "].";
        shader->setVec3(base + "position", pointLights[i]->getPosition());
        shader->setVec3(base + "color", pointLights[i]->getColor());
        shader->setFloat(base + "intensity", pointLights[i]->getIntensity());
        shader->setFloat(base + "range", pointLights[i]->getRange());
        
        // Attenuation factors
        float range = pointLights[i]->getRange();
        shader->setFloat(base + "constant", 1.0f);
        shader->setFloat(base + "linear", 2.0f / range);
        shader->setFloat(base + "quadratic", 1.0f / (range * range));
    }
}

void LightManager::applySpotLights(Shader* shader) {
    shader->setInt("numSpotLights", static_cast<int>(spotLights.size()));

    for (size_t i = 0; i < spotLights.size() && i < MAX_SPOT_LIGHTS; ++i) {
        std::string base = "spotLights[" + std::to_string(i) + "].";
        shader->setVec3(base + "position", spotLights[i]->getPosition());
        shader->setVec3(base + "direction", spotLights[i]->getDirection());
        shader->setVec3(base + "color", spotLights[i]->getColor());
        shader->setFloat(base + "intensity", spotLights[i]->getIntensity());
        shader->setFloat(base + "range", spotLights[i]->getRange());
        
        float angle = spotLights[i]->getSpotAngle();
        shader->setFloat(base + "cutOff", glm::cos(glm::radians(angle)));
        shader->setFloat(base + "outerCutOff", glm::cos(glm::radians(angle + 5.0f)));
    }
}
