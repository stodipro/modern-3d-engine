#include "UISystem.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void UISystem::initialize(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup style
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 4.0f;
    style.WindowRounding = 4.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);

    // Setup platform/renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");
}

void UISystem::shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UISystem::beginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UISystem::endFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UISystem::beginWindow(const std::string& name) {
    ImGui::Begin(name.c_str());
}

void UISystem::endWindow() {
    ImGui::End();
}

bool UISystem::button(const std::string& label) {
    return ImGui::Button(label.c_str());
}

void UISystem::text(const std::string& text) {
    ImGui::Text("%s", text.c_str());
}

bool UISystem::slider(const std::string& label, float& value, float min, float max) {
    return ImGui::SliderFloat(label.c_str(), &value, min, max);
}

bool UISystem::slider(const std::string& label, int& value, int min, int max) {
    return ImGui::SliderInt(label.c_str(), &value, min, max);
}

bool UISystem::colorPicker(const std::string& label, float color[3]) {
    return ImGui::ColorEdit3(label.c_str(), color);
}

bool UISystem::comboBox(const std::string& label, int& currentItem, const std::vector<std::string>& items) {
    if (ImGui::BeginCombo(label.c_str(), items[currentItem].c_str())) {
        bool changed = false;
        for (int i = 0; i < items.size(); i++) {
            bool isSelected = (currentItem == i);
            if (ImGui::Selectable(items[i].c_str(), isSelected)) {
                currentItem = i;
                changed = true;
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
        return changed;
    }
    return false;
}

bool UISystem::checkbox(const std::string& label, bool& value) {
    return ImGui::Checkbox(label.c_str(), &value);
}

void UISystem::sameLine() {
    ImGui::SameLine();
}

void UISystem::separator() {
    ImGui::Separator();
}

void UISystem::spacing() {
    ImGui::Spacing();
}
