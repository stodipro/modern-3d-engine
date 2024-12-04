#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

class Shader;
class Texture;

class Material {
public:
    Material();
    explicit Material(std::shared_ptr<Shader> shader);

    void bind();
    void unbind();

    // Shader management
    void setShader(std::shared_ptr<Shader> shader);
    Shader* getShader() const { return shader.get(); }

    // Transform matrices
    void setModelMatrix(const glm::mat4& matrix);
    void setViewMatrix(const glm::mat4& matrix);
    void setProjectionMatrix(const glm::mat4& matrix);

    // Material properties
    void setFloat(const std::string& name, float value);
    void setInt(const std::string& name, int value);
    void setVector2(const std::string& name, const glm::vec2& value);
    void setVector3(const std::string& name, const glm::vec3& value);
    void setVector4(const std::string& name, const glm::vec4& value);
    void setMatrix4(const std::string& name, const glm::mat4& value);
    void setTexture(const std::string& name, std::shared_ptr<Texture> texture);

private:
    std::shared_ptr<Shader> shader;
    std::unordered_map<std::string, std::shared_ptr<Texture>> textures;

    // Material properties cache
    std::unordered_map<std::string, float> floatProperties;
    std::unordered_map<std::string, int> intProperties;
    std::unordered_map<std::string, glm::vec2> vector2Properties;
    std::unordered_map<std::string, glm::vec3> vector3Properties;
    std::unordered_map<std::string, glm::vec4> vector4Properties;
    std::unordered_map<std::string, glm::mat4> matrix4Properties;

    void applyProperties();
};
