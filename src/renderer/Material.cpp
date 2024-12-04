#include "Material.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

Material::Material() : shader(nullptr) {}

Material::Material(std::shared_ptr<Shader> shader) : shader(shader) {}

void Material::bind() {
    if (shader) {
        shader->use();
        applyProperties();

        // Bind textures
        int textureUnit = 0;
        for (const auto& [name, texture] : textures) {
            if (texture) {
                texture->bind(textureUnit);
                shader->setInt(name, textureUnit);
                textureUnit++;
            }
        }
    }
}

void Material::unbind() {
    // Unbind textures
    for (const auto& [name, texture] : textures) {
        if (texture) {
            texture->unbind();
        }
    }
}

void Material::setShader(std::shared_ptr<Shader> newShader) {
    shader = newShader;
}

void Material::setModelMatrix(const glm::mat4& matrix) {
    if (shader) {
        shader->setMat4("model", matrix);
    }
}

void Material::setViewMatrix(const glm::mat4& matrix) {
    if (shader) {
        shader->setMat4("view", matrix);
    }
}

void Material::setProjectionMatrix(const glm::mat4& matrix) {
    if (shader) {
        shader->setMat4("projection", matrix);
    }
}

void Material::setFloat(const std::string& name, float value) {
    floatProperties[name] = value;
}

void Material::setInt(const std::string& name, int value) {
    intProperties[name] = value;
}

void Material::setVector2(const std::string& name, const glm::vec2& value) {
    vector2Properties[name] = value;
}

void Material::setVector3(const std::string& name, const glm::vec3& value) {
    vector3Properties[name] = value;
}

void Material::setVector4(const std::string& name, const glm::vec4& value) {
    vector4Properties[name] = value;
}

void Material::setMatrix4(const std::string& name, const glm::mat4& value) {
    matrix4Properties[name] = value;
}

void Material::setTexture(const std::string& name, std::shared_ptr<Texture> texture) {
    textures[name] = texture;
}

void Material::applyProperties() {
    if (!shader) return;

    for (const auto& [name, value] : floatProperties) {
        shader->setFloat(name, value);
    }

    for (const auto& [name, value] : intProperties) {
        shader->setInt(name, value);
    }

    for (const auto& [name, value] : vector2Properties) {
        shader->setVec2(name, value);
    }

    for (const auto& [name, value] : vector3Properties) {
        shader->setVec3(name, value);
    }

    for (const auto& [name, value] : vector4Properties) {
        shader->setVec4(name, value);
    }

    for (const auto& [name, value] : matrix4Properties) {
        shader->setMat4(name, value);
    }
}
