#pragma once
#include "UISystem.hpp"
#include "../sculpting/SculptingSystem.hpp"

class SculptingUI {
public:
    static SculptingUI& getInstance() {
        static SculptingUI instance;
        return instance;
    }

    void initialize();
    void render();

    // UI state
    bool isVisible() const { return visible; }
    void setVisible(bool value) { visible = value; }
    void toggleVisible() { visible = !visible; }

private:
    SculptingUI() = default;
    ~SculptingUI() = default;
    SculptingUI(const SculptingUI&) = delete;
    SculptingUI& operator=(const SculptingUI&) = delete;

    bool visible;
    int currentTool;
    float toolRadius;
    float toolStrength;
    bool symmetryEnabled;
    int symmetryAxis; // 0=X, 1=Y, 2=Z

    // UI sections
    void renderToolSelector();
    void renderToolSettings();
    void renderSymmetrySettings();
    void renderMeshInfo();
};
