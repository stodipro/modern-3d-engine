#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <array>

class Input {
public:
    static Input& getInstance() {
        static Input instance;
        return instance;
    }

    void initialize(GLFWwindow* window);
    void update();

    // Keyboard input
    bool isKeyPressed(int key) const;
    bool isKeyJustPressed(int key) const;
    bool isKeyReleased(int key) const;

    // Mouse input
    bool isMouseButtonPressed(int button) const;
    bool isMouseButtonJustPressed(int button) const;
    bool isMouseButtonReleased(int button) const;
    const glm::vec2& getMousePosition() const;
    const glm::vec2& getMouseDelta() const;
    float getMouseScrollDelta() const;

    // Utility functions
    void setCursorMode(bool locked);

private:
    Input() = default;
    ~Input() = default;
    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;

    GLFWwindow* window;
    
    // Keyboard state
    std::unordered_map<int, bool> currentKeyState;
    std::unordered_map<int, bool> previousKeyState;

    // Mouse state
    std::array<bool, GLFW_MOUSE_BUTTON_LAST> currentMouseState;
    std::array<bool, GLFW_MOUSE_BUTTON_LAST> previousMouseState;
    glm::vec2 mousePosition;
    glm::vec2 previousMousePosition;
    glm::vec2 mouseDelta;
    float scrollDelta;

    // Callback functions
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};
