#include "UndoSystem.hpp"

void UndoSystem::initialize(size_t maxStates) {
    maxUndoStates = maxStates;
    clear();
}

void UndoSystem::clear() {
    undoStates.clear();
    redoStates.clear();
}

void UndoSystem::saveState(const SculptMesh& mesh) {
    redoStates.clear(); // Clear redo stack when new action is performed
    pushState(createState(mesh));
}

bool UndoSystem::undo() {
    if (!canUndo()) return false;

    auto currentMesh = SculptingSystem::getInstance().getTargetMesh();
    if (!currentMesh) return false;

    // Save current state to redo stack
    redoStates.push_back(createState(*currentMesh));

    // Apply previous state
    MeshState state = undoStates.back();
    undoStates.pop_back();
    applyState(state, *currentMesh);

    return true;
}

bool UndoSystem::redo() {
    if (!canRedo()) return false;

    auto currentMesh = SculptingSystem::getInstance().getTargetMesh();
    if (!currentMesh) return false;

    // Save current state to undo stack
    undoStates.push_back(createState(*currentMesh));

    // Apply redo state
    MeshState state = redoStates.back();
    redoStates.pop_back();
    applyState(state, *currentMesh);

    return true;
}

bool UndoSystem::canUndo() const {
    return !undoStates.empty();
}

bool UndoSystem::canRedo() const {
    return !redoStates.empty();
}

void UndoSystem::pushState(const MeshState& state) {
    undoStates.push_back(state);
    if (undoStates.size() > maxUndoStates) {
        undoStates.erase(undoStates.begin());
    }
}

UndoSystem::MeshState UndoSystem::createState(const SculptMesh& mesh) {
    MeshState state;
    state.vertices = mesh.getVertices();
    state.indices = mesh.getIndices();
    return state;
}

void UndoSystem::applyState(const MeshState& state, SculptMesh& mesh) {
    mesh.setVertices(state.vertices);
    mesh.setIndices(state.indices);
    mesh.recalculateNormals();
}
