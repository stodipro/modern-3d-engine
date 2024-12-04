#include "Input.hpp"

void Input::initialize(GLFWwindow* window) {
    this->window = window;

    // Set callbacks
    glfwSetWindowUserPointer(window, this);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);

    // Initialize mouse state
    currentMouseState.fill(false);
    previousMouseState.fill(false);
    mousePosition = glm::vec2(0.0f);
    previousMousePosition = glm::vec2(0.0f);
    mouseDelta = glm::vec2(0.0f);
    scrollDelta = 0.0f;
}

void Input::update() {
    // Update keyboard state
    previousKeyState = currentKeyState;
    
    // Update mouse state
    previousMouseState = currentMouseState;
    previousMousePosition = mousePosition;
    scrollDelta = 0.0f; // Reset scroll delta each frame

    // Calculate mouse delta
    mouseDelta = mousePosition - previousMousePosition;
}

bool Input::isKeyPressed(int key) const {
    auto it = currentKeyState.find(key);
    return it != currentKeyState.end() && it->second;
}

bool Input::isKeyJustPressed(int key) const {
    auto curr = currentKeyState.find(key);
    auto prev = previousKeyState.find(key);
    return (curr != currentKeyState.end() && curr->second) &&
           (prev == previousKeyState.end() || !prev->second);
}

bool Input::isKeyReleased(int key) const {
    auto it = currentKeyState.find(key);
    return it == currentKeyState.end() || !it->second;
}

bool Input::isMouseButtonPressed(int button) const {
    return button < GLFW_MOUSE_BUTTON_LAST && currentMouseState[button];
}

bool Input::isMouseButtonJustPressed(int button) const {
    return button < GLFW_MOUSE_BUTTON_LAST &&
           currentMouseState[button] && !previousMouseState[button];
}

bool Input::isMouseButtonReleased(int button) const {
    return button < GLFW_MOUSE_BUTTON_LAST && !currentMouseState[button];
}

const glm::vec2& Input::getMousePosition() const {
    return mousePosition;
}

const glm::vec2& Input::getMouseDelta() const {
    return mouseDelta;
}

float Input::getMouseScrollDelta() const {
    return scrollDelta;
}

void Input::setCursorMode(bool locked) {
    glfwSetInputMode(window, GLFW_CURSOR,
                     locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void Input::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    auto input = static_cast<Input*>(glfwGetWindowUserPointer(window));
    if (button < GLFW_MOUSE_BUTTON_LAST) {
        input->currentMouseState[button] = (action == GLFW_PRESS);
    }
}

void Input::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    auto input = static_cast<Input*>(glfwGetWindowUserPointer(window));
    input->mousePosition = glm::vec2(xpos, ypos);
}

void Input::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    auto input = static_cast<Input*>(glfwGetWindowUserPointer(window));
    input->scrollDelta = static_cast<float>(yoffset);
}
