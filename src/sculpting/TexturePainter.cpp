#include "TexturePainter.hpp"
#include <algorithm>
#include <cmath>

float TexturePainter::calculateBrushStrength(float distance) {
    if (distance >= brushRadius) return 0.0f;
    
    float normalizedDist = distance / brushRadius;
    float falloff;
    
    if (brushHardness > 0.99f) {
        falloff = normalizedDist < 0.99f ? 1.0f : 0.0f;
    } else {
        float hardnessScale = 1.0f - brushHardness;
        falloff = 1.0f - std::pow(normalizedDist, 1.0f / hardnessScale);
    }
    
    return glm::clamp(falloff, 0.0f, 1.0f);
}

void TexturePainter::applyBrush(const glm::vec2& uv, const std::function<void(int x, int y, float strength)>& operation) {
    // Convert UV to pixel coordinates
    int centerX = static_cast<int>(uv.x * textureWidth);
    int centerY = static_cast<int>(uv.y * textureHeight);
    
    // Calculate brush bounds
    int minX = std::max(0, centerX - static_cast<int>(brushRadius));
    int maxX = std::min(textureWidth - 1, centerX + static_cast<int>(brushRadius));
    int minY = std::max(0, centerY - static_cast<int>(brushRadius));
    int maxY = std::min(textureHeight - 1, centerY + static_cast<int>(brushRadius));
    
    // Apply brush operation to affected pixels
    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            float dx = x - centerX;
            float dy = y - centerY;
            float distance = std::sqrt(dx * dx + dy * dy);
            
            float strength = calculateBrushStrength(distance);
            if (strength > 0.0f) {
                operation(x, y, strength);
            }
        }
    }
}

glm::vec4 TexturePainter::getPixel(int layerIndex, int x, int y) {
    if (layerIndex < 0 || layerIndex >= layers.size()) return glm::vec4(0.0f);
    
    // Get raw pixel data from texture
    unsigned char* data = layers[layerIndex].texture->getData();
    int index = (y * textureWidth + x) * 4;
    
    return glm::vec4(
        data[index] / 255.0f,
        data[index + 1] / 255.0f,
        data[index + 2] / 255.0f,
        data[index + 3] / 255.0f
    );
}

void TexturePainter::setPixel(int layerIndex, int x, int y, const glm::vec4& color) {
    if (layerIndex < 0 || layerIndex >= layers.size()) return;
    
    unsigned char* data = layers[layerIndex].texture->getData();
    int index = (y * textureWidth + x) * 4;
    
    data[index] = static_cast<unsigned char>(color.r * 255.0f);
    data[index + 1] = static_cast<unsigned char>(color.g * 255.0f);
    data[index + 2] = static_cast<unsigned char>(color.b * 255.0f);
    data[index + 3] = static_cast<unsigned char>(color.a * 255.0f);
}

void TexturePainter::updateFinalTexture() {
    // Create temporary buffer for final result
    std::vector<unsigned char> finalData(textureWidth * textureHeight * 4, 0);
    
    // Blend all visible layers
    for (const auto& layer : layers) {
        if (!layer.visible) continue;
        
        unsigned char* layerData = layer.texture->getData();
        for (int i = 0; i < textureWidth * textureHeight * 4; i += 4) {
            // Get source and destination colors
            glm::vec4 srcColor(
                layerData[i] / 255.0f,
                layerData[i + 1] / 255.0f,
                layerData[i + 2] / 255.0f,
                layerData[i + 3] / 255.0f * layer.opacity
            );
            
            glm::vec4 dstColor(
                finalData[i] / 255.0f,
                finalData[i + 1] / 255.0f,
                finalData[i + 2] / 255.0f,
                finalData[i + 3] / 255.0f
            );
            
            // Blend colors based on blend mode
            glm::vec4 result = blendColors(srcColor, dstColor, layer.blendMode);
            
            // Write back to final buffer
            finalData[i] = static_cast<unsigned char>(result.r * 255.0f);
            finalData[i + 1] = static_cast<unsigned char>(result.g * 255.0f);
            finalData[i + 2] = static_cast<unsigned char>(result.b * 255.0f);
            finalData[i + 3] = static_cast<unsigned char>(result.a * 255.0f);
        }
    }
    
    // Update final texture
    finalTexture->update(finalData.data());
}

glm::vec4 TexturePainter::blendColors(const glm::vec4& src, const glm::vec4& dst, int blendMode) {
    switch (blendMode) {
        case 0: // Normal
            return glm::vec4(
                src.rgb() * src.a + dst.rgb() * (1.0f - src.a),
                src.a + dst.a * (1.0f - src.a)
            );
            
        case 1: // Multiply
            return glm::vec4(
                src.rgb() * dst.rgb(),
                src.a * dst.a
            );
            
        case 2: // Add
            return glm::vec4(
                glm::min(src.rgb() + dst.rgb(), glm::vec3(1.0f)),
                glm::min(src.a + dst.a, 1.0f)
            );
            
        // Add more blend modes as needed
            
        default:
            return src;
    }
}
