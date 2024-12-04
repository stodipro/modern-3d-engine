#pragma once
#include "../src/core/Engine.hpp"
#include "../src/sculpting/SculptingSystem.hpp"
#include <memory>

class SculptingDemo {
public:
    SculptingDemo();
    ~SculptingDemo();

    bool initialize();
    void update(float deltaTime);
    void render();

private:
    Engine engine;
    std::shared_ptr<Entity> cameraEntity;
    std::shared_ptr<Entity> sculptMeshEntity;
    std::shared_ptr<SculptMesh> sculptMesh;

    void setupCamera();
    void setupSculptMesh();
    void setupLighting();
    void handleInput(float deltaTime);
    void updateUI();
};
