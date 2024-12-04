#include "PaintingUI.hpp"

void PaintingUI::initialize() {
    visible = true;
    currentBrush = TexturePainter::BrushType::Color;
    brushColor = glm::vec4(1.0f);
    brushRadius = 10.0f;
    brushStrength = 1.0f;
    brushHardness = 0.5f;
    selectedLayer = 0;

    // Add default layer
    LayerUI baseLayer;
    baseLayer.name = "Base Layer";
    baseLayer.visible = true;
    baseLayer.opacity = 1.0f;
    baseLayer.blendMode = 0;
    layers.push_back(baseLayer);
}

void PaintingUI::render() {
    if (!visible) return;

    auto& ui = UISystem::getInstance();
    
    // Main painting window
    ui.beginWindow("Texture Painting");

    // Brush settings
    renderBrushSettings();
    ui.separator();

    // Color picker
    renderColorPicker();
    ui.separator();

    // Layer panel
    renderLayerPanel();

    ui.endWindow();

    // Texture preview window
    renderTexturePreview();
}

void PaintingUI::renderBrushSettings() {
    auto& ui = UISystem::getInstance();

    ui.text("Brush Settings");
    ui.spacing();

    // Brush type selector
    std::vector<std::string> brushTypes = {
        "Color",
        "Smooth",
        "Smear",
        "Clone"
    };
    int brushTypeIndex = static_cast<int>(currentBrush);
    if (ui.comboBox("Brush Type", brushTypeIndex, brushTypes)) {
        currentBrush = static_cast<TexturePainter::BrushType>(brushTypeIndex);
    }

    // Brush parameters
    ui.slider("Radius", brushRadius, 1.0f, 100.0f);
    ui.slider("Strength", brushStrength, 0.0f, 1.0f);
    ui.slider("Hardness", brushHardness, 0.0f, 1.0f);
}

void PaintingUI::renderColorPicker() {
    auto& ui = UISystem::getInstance();

    ui.text("Color");
    ui.spacing();

    float color[4] = { brushColor.r, brushColor.g, brushColor.b, brushColor.a };
    if (ui.colorPicker("Brush Color", color)) {
        brushColor = glm::vec4(color[0], color[1], color[2], color[3]);
    }
}

void PaintingUI::renderLayerPanel() {
    auto& ui = UISystem::getInstance();

    ui.text("Layers");
    ui.spacing();

    // Add layer button
    if (ui.button("Add Layer")) {
        LayerUI newLayer;
        newLayer.name = "Layer " + std::to_string(layers.size());
        newLayer.visible = true;
        newLayer.opacity = 1.0f;
        newLayer.blendMode = 0;
        layers.push_back(newLayer);
    }

    // Layer list
    for (int i = layers.size() - 1; i >= 0; --i) {
        auto& layer = layers[i];

        // Layer selection
        bool selected = (selectedLayer == i);
        if (ui.selectable(layer.name, selected)) {
            selectedLayer = i;
        }

        ui.sameLine();

        // Visibility toggle
        ui.checkbox("##visible" + std::to_string(i), layer.visible);

        // Layer settings
        if (selected) {
            ui.slider("Opacity##" + std::to_string(i), layer.opacity, 0.0f, 1.0f);
            
            std::vector<std::string> blendModes = {
                "Normal",
                "Multiply",
                "Add"
            };
            ui.comboBox("Blend Mode##" + std::to_string(i), layer.blendMode, blendModes);

            if (i > 0 && ui.button("Delete Layer##" + std::to_string(i))) {
                layers.erase(layers.begin() + i);
                if (selectedLayer >= layers.size()) {
                    selectedLayer = layers.size() - 1;
                }
            }
        }
    }
}

void PaintingUI::renderTexturePreview() {
    auto& ui = UISystem::getInstance();

    ui.beginWindow("Texture Preview");

    // TODO: Add texture preview rendering
    // This would show the current layer or final composite
    // with proper UV mapping visualization

    ui.endWindow();
}
