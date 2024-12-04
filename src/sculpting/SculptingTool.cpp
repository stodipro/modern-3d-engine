#include "SculptingTool.hpp"

SculptingTool::SculptingTool()
    : type(Type::Pull)
    , radius(1.0f)
    , strength(0.5f)
{}

void SculptingTool::apply(SculptMesh& mesh, const glm::vec3& position) {
    switch (type) {
        case Type::Pull:
            mesh.pull(position, radius, strength);
            break;
        case Type::Push:
            mesh.push(position, radius, strength);
            break;
        case Type::Smooth:
            mesh.smooth(position, radius, strength);
            break;
        case Type::Pinch:
            mesh.pinch(position, radius, strength);
            break;
    }
}
