#pragma once
#include <GLFW/glfw3.h>
#include <string>
#include <functional>
#include <vector>

class UISystem {
public:
    static UISystem& getInstance() {
        static UISystem instance;
        return instance;
    }

    void initialize(GLFWwindow* window);
    void shutdown();

    void beginFrame();
    void endFrame();

    // Window creation helpers
    void beginWindow(const std::string& name);
    void endWindow();

    // Basic widgets
    bool button(const std::string& label);
    void text(const std::string& text);
    bool slider(const std::string& label, float& value, float min, float max);
    bool slider(const std::string& label, int& value, int min, int max);
    bool colorPicker(const std::string& label, float color[3]);
    bool comboBox(const std::string& label, int& currentItem, const std::vector<std::string>& items);
    bool checkbox(const std::string& label, bool& value);

    // Layout helpers
    void sameLine();
    void separator();
    void spacing();

private:
    UISystem() = default;
    ~UISystem() = default;
    UISystem(const UISystem&) = delete;
    UISystem& operator=(const UISystem&) = delete;
};
