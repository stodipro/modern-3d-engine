#pragma once
#include <vector>
#include <memory>
#include "SculptMesh.hpp"

class UndoSystem {
public:
    static UndoSystem& getInstance() {
        static UndoSystem instance;
        return instance;
    }

    void initialize(size_t maxStates = 50);
    void clear();

    // State management
    void saveState(const SculptMesh& mesh);
    bool undo();
    bool redo();
    bool canUndo() const;
    bool canRedo() const;

    // Get current state
    std::shared_ptr<SculptMesh> getCurrentState();

private:
    UndoSystem() = default;
    ~UndoSystem() = default;
    UndoSystem(const UndoSystem&) = delete;
    UndoSystem& operator=(const UndoSystem&) = delete;

    struct MeshState {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
    };

    std::vector<MeshState> undoStates;
    std::vector<MeshState> redoStates;
    size_t maxUndoStates;

    void pushState(const MeshState& state);
    MeshState createState(const SculptMesh& mesh);
    void applyState(const MeshState& state, SculptMesh& mesh);
};
