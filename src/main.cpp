#include "examples/DemoScene.hpp"
#include <iostream>

int main() {
    DemoScene demo;

    if (!demo.initialize()) {
        std::cerr << "Failed to initialize demo scene" << std::endl;
        return -1;
    }

    // Main game loop
    float lastTime = 0.0f;
    while (true) {
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        demo.update(deltaTime);
        demo.render();

        // Break the loop if the window should close
        if (glfwWindowShouldClose(glfwGetCurrentContext())) {
            break;
        }
    }

    return 0;
}
