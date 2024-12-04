#pragma once
#include "../src/core/Engine.hpp"
#include "../src/core/Input.hpp"
#include "../src/components/Camera.hpp"
#include <memory>

class DemoScene {
public:
    DemoScene();
    ~DemoScene();

    bool initialize();
    void update(float deltaTime);
    void render();

private:
    Engine engine;
    std::shared_ptr<Entity> cameraEntity;
    std::shared_ptr<Entity> modelEntity;

    void setupCamera();
    void setupModel();
    void handleInput(float deltaTime);
};
