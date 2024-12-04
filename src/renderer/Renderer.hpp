#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class Scene;
class Camera;

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool initialize();
    void shutdown();
    
    void beginFrame();
    void render(const Scene& scene);
    void endFrame();

    void setViewport(int width, int height);
    void setClearColor(const glm::vec4& color);

private:
    glm::vec4 clearColor;
};
