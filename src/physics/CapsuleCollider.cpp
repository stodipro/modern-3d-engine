#include "CapsuleCollider.hpp"
#include "../components/Transform.hpp"
#include <glm/gtx/closest_point.hpp>

CapsuleCollider::CapsuleCollider()
    : Collider(Type::Capsule)
    , radius(0.5f)
    , height(2.0f)
{}

void CapsuleCollider::getSegment(glm::vec3& start, glm::vec3& end) const {
    auto transform = getEntity()->getComponent<Transform>();
    glm::vec3 pos = transform->getPosition();
    glm::vec3 up = transform->getUp();
    
    float halfHeight = height * 0.5f;
    start = pos - up * halfHeight;
    end = pos + up * halfHeight;
}

bool CapsuleCollider::checkCollision(const Collider* other, glm::vec3& contactPoint, glm::vec3& normal, float& depth) const {
    switch (other->getType()) {
        case Type::Capsule:
            return capsuleCapsule(static_cast<const CapsuleCollider*>(other), contactPoint, normal, depth);
        case Type::Sphere:
            return capsuleSphere(static_cast<const SphereCollider*>(other), contactPoint, normal, depth);
        case Type::Box:
            return capsuleBox(static_cast<const BoxCollider*>(other), contactPoint, normal, depth);
        default:
            return false;
    }
}

bool CapsuleCollider::capsuleCapsule(const CapsuleCollider* other, glm::vec3& contactPoint, 
                                    glm::vec3& normal, float& depth) const {
    glm::vec3 start1, end1, start2, end2;
    getSegment(start1, end1);
    other->getSegment(start2, end2);

    // Find closest points between the two central axes
    glm::vec3 d1 = end1 - start1;
    glm::vec3 d2 = end2 - start2;
    glm::vec3 r = start1 - start2;

    float a = glm::dot(d1, d1);
    float b = glm::dot(d1, d2);
    float c = glm::dot(d2, d2);
    float d = glm::dot(d1, r);
    float e = glm::dot(d2, r);
    float f = glm::dot(r, r);

    float denom = a * c - b * b;
    float t1 = 0.0f;
    float t2 = 0.0f;

    if (denom > 0.0001f) {
        t1 = glm::clamp((b * e - c * d) / denom, 0.0f, 1.0f);
        t2 = (b * t1 + e) / c;
        if (t2 < 0.0f) {
            t2 = 0.0f;
            t1 = glm::clamp(-d / a, 0.0f, 1.0f);
        } else if (t2 > 1.0f) {
            t2 = 1.0f;
            t1 = glm::clamp((b - d) / a, 0.0f, 1.0f);
        }
    }

    glm::vec3 c1 = start1 + d1 * t1;
    glm::vec3 c2 = start2 + d2 * t2;

    // Check distance between closest points
    glm::vec3 dist = c2 - c1;
    float distLen = glm::length(dist);
    float minDist = radius + other->radius;

    if (distLen < minDist) {
        depth = minDist - distLen;
        normal = distLen > 0.0001f ? dist / distLen : glm::vec3(0, 1, 0);
        contactPoint = c1 + normal * radius;
        return true;
    }

    return false;
}

bool CapsuleCollider::capsuleSphere(const SphereCollider* sphere, glm::vec3& contactPoint,
                                   glm::vec3& normal, float& depth) const {
    glm::vec3 start, end;
    getSegment(start, end);

    // Get sphere center
    auto sphereTransform = sphere->getEntity()->getComponent<Transform>();
    glm::vec3 center = sphereTransform->getPosition();

    // Find closest point on capsule line segment to sphere center
    glm::vec3 d = end - start;
    float t = glm::clamp(glm::dot(center - start, d) / glm::dot(d, d), 0.0f, 1.0f);
    glm::vec3 closest = start + d * t;

    // Check collision
    glm::vec3 dist = center - closest;
    float distLen = glm::length(dist);
    float minDist = radius + sphere->getRadius();

    if (distLen < minDist) {
        depth = minDist - distLen;
        normal = distLen > 0.0001f ? dist / distLen : glm::vec3(0, 1, 0);
        contactPoint = closest + normal * radius;
        return true;
    }

    return false;
}

bool CapsuleCollider::capsuleBox(const BoxCollider* box, glm::vec3& contactPoint,
                                glm::vec3& normal, float& depth) const {
    // For now, using a simplified box-capsule collision
    // Could be improved with more accurate box-capsule collision detection
    glm::vec3 start, end;
    getSegment(start, end);

    auto boxTransform = box->getEntity()->getComponent<Transform>();
    glm::vec3 boxPos = boxTransform->getPosition();
    glm::vec3 boxSize = box->getSize();

    // Find closest point on box to capsule line segment
    glm::vec3 closest = glm::clamp(
        start,
        boxPos - boxSize * 0.5f,
        boxPos + boxSize * 0.5f
    );

    glm::vec3 dist = closest - start;
    float distLen = glm::length(dist);

    if (distLen < radius) {
        depth = radius - distLen;
        normal = distLen > 0.0001f ? dist / distLen : glm::vec3(0, 1, 0);
        contactPoint = start + normal * radius;
        return true;
    }

    return false;
}

void CapsuleCollider::calculateBounds(glm::vec3& min, glm::vec3& max) const {
    auto transform = getEntity()->getComponent<Transform>();
    glm::vec3 pos = transform->getPosition();
    glm::vec3 extent(radius);
    extent.y += height * 0.5f;

    min = pos - extent;
    max = pos + extent;
}
