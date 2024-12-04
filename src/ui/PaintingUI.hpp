#pragma once
#include "UISystem.hpp"
#include "../sculpting/TexturePainter.hpp"
#include <memory>

class PaintingUI {
public:
    static PaintingUI& getInstance() {
        static PaintingUI instance;
        return instance;
    }

    void initialize();
    void render();

    bool isVisible() const { return visible; }
    void setVisible(bool value) { visible = value; }
    void toggleVisible() { visible = !visible; }

private:
    PaintingUI() = default;
    ~PaintingUI() = default;
    PaintingUI(const PaintingUI&) = delete;
    PaintingUI& operator=(const PaintingUI&) = delete;

    bool visible;
    TexturePainter::BrushType currentBrush;
    glm::vec4 brushColor;
    float brushRadius;
    float brushStrength;
    float brushHardness;
    int selectedLayer;

    // Layer management
    struct LayerUI {
        std::string name;
        bool visible;
        float opacity;
        int blendMode;
    };
    std::vector<LayerUI> layers;

    // UI sections
    void renderBrushSettings();
    void renderColorPicker();
    void renderLayerPanel();
    void renderTexturePreview();
};
