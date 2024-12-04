#pragma once
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window {
public:
    Window();
    ~Window();
    
    bool initialize(const std::string& title, int width, int height);
    void shutdown();
    void pollEvents();
    void swapBuffers();
    bool shouldClose() const;
    
    GLFWwindow* getGLFWWindow() { return window; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    GLFWwindow* window;
    int width;
    int height;
    
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
};