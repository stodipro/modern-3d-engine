#include "SculptingUI.hpp"

void SculptingUI::initialize() {
    visible = true;
    currentTool = 0;
    toolRadius = 1.0f;
    toolStrength = 0.5f;
    symmetryEnabled = false;
    symmetryAxis = 0;
}

void SculptingUI::render() {
    if (!visible) return;

    auto& ui = UISystem::getInstance();
    
    ui.beginWindow("Sculpting Tools");

    // Tool selection
    renderToolSelector();
    ui.separator();

    // Tool settings
    renderToolSettings();
    ui.separator();

    // Symmetry settings
    renderSymmetrySettings();
    ui.separator();

    // Mesh information
    renderMeshInfo();

    ui.endWindow();
}

void SculptingUI::renderToolSelector() {
    auto& ui = UISystem::getInstance();
    auto& sculptingSystem = SculptingSystem::getInstance();

    ui.text("Tool Selection");
    ui.spacing();

    std::vector<std::string> tools = {
        "Pull",
        "Push",
        "Smooth",
        "Pinch"
    };

    if (ui.comboBox("Active Tool", currentTool, tools)) {
        sculptingSystem.setActiveTool(static_cast<SculptingTool::Type>(currentTool));
    }

    // Quick select buttons
    if (ui.button("Pull (1)")) {
        currentTool = 0;
        sculptingSystem.setActiveTool(SculptingTool::Type::Pull);
    }
    ui.sameLine();
    if (ui.button("Push (2)")) {
        currentTool = 1;
        sculptingSystem.setActiveTool(SculptingTool::Type::Push);
    }
    ui.sameLine();
    if (ui.button("Smooth (3)")) {
        currentTool = 2;
        sculptingSystem.setActiveTool(SculptingTool::Type::Smooth);
    }
    ui.sameLine();
    if (ui.button("Pinch (4)")) {
        currentTool = 3;
        sculptingSystem.setActiveTool(SculptingTool::Type::Pinch);
    }
}

void SculptingUI::renderToolSettings() {
    auto& ui = UISystem::getInstance();
    auto& sculptingSystem = SculptingSystem::getInstance();

    ui.text("Tool Settings");
    ui.spacing();

    if (ui.slider("Radius", toolRadius, 0.1f, 5.0f)) {
        sculptingSystem.setToolRadius(toolRadius);
    }

    if (ui.slider("Strength", toolStrength, 0.0f, 1.0f)) {
        sculptingSystem.setToolStrength(toolStrength);
    }
}

void SculptingUI::renderSymmetrySettings() {
    auto& ui = UISystem::getInstance();

    ui.text("Symmetry");
    ui.spacing();

    if (ui.checkbox("Enable Symmetry", symmetryEnabled)) {
        // TODO: Implement symmetry in sculpting system
    }

    if (symmetryEnabled) {
        std::vector<std::string> axes = {"X Axis", "Y Axis", "Z Axis"};
        if (ui.comboBox("Symmetry Axis", symmetryAxis, axes)) {
            // TODO: Update symmetry axis
        }
    }
}

void SculptingUI::renderMeshInfo() {
    auto& ui = UISystem::getInstance();
    auto& sculptingSystem = SculptingSystem::getInstance();

    ui.text("Mesh Information");
    ui.spacing();

    if (auto mesh = sculptingSystem.getTargetMesh()) {
        ui.text("Vertices: " + std::to_string(mesh->getVertices().size()));
        ui.text("Triangles: " + std::to_string(mesh->getIndices().size() / 3));
    }

    if (ui.button("Subdivide Mesh")) {
        if (auto mesh = sculptingSystem.getTargetMesh()) {
            mesh->subdivide(1.0f); // Threshold of 1.0 for uniform subdivision
        }
    }
}
