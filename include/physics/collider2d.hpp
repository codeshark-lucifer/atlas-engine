#pragma once
#include <ecs/ecs.hpp>
#include <memory>
#include "comp.h"

enum class CollistionObject : uint32_t
{
    None = 0,
    Static = 1 << 0,
    Dynamic = 1 << 1,
    Kinematic = 1 << 2,
    Bullet = 1 << 3,
    CanSleep = 1 << 4
};

inline CollistionObject operator|(CollistionObject a, CollistionObject b)
{
    return static_cast<CollistionObject>(
        static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline CollistionObject operator&(CollistionObject a, CollistionObject b)
{
    return static_cast<CollistionObject>(
        static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

namespace physics
{
    struct MotionState
    {
        vec2 position;
        quat quaternion;

        static MotionState create(const vec2 &p, const quat &q)
        {
            return MotionState{p, q};
        }
    };

    struct PhysicsShape
    {
        virtual ~PhysicsShape() = default;
        // Calculate and return the moment of inertia (scalar) for given mass
        virtual float CalculateLocalInertia(float mass) const = 0;
    };

    struct ShapeBox : public PhysicsShape
    {
        vec2 halfSize;

        ShapeBox(const vec2 &halfSize) : halfSize(halfSize) {}

        // Rectangle moment of inertia about center: (1/12) * m * (w^2 + h^2)
        float CalculateLocalInertia(float mass) const override
        {
            float w = 2.0f * halfSize.x;
            float h = 2.0f * halfSize.y;
            return (1.0f / 12.0f) * mass * (w * w + h * h);
        }
    };

    struct ShapeCircle : public PhysicsShape
    {
        float radius;

        ShapeCircle(float radius) : radius(radius) {}

        // Solid disk: I = 1/2 * m * r^2
        float CalculateLocalInertia(float mass) const override
        {
            return 0.5f * mass * radius * radius;
        }
    };

    struct ShapeTriangle : public PhysicsShape
    {
        vec2 vertices[3];  // Three vertices in local space

        ShapeTriangle(const vec2 &v0, const vec2 &v1, const vec2 &v2)
        {
            vertices[0] = v0;
            vertices[1] = v1;
            vertices[2] = v2;
        }

        // Triangle moment of inertia: (1/18) * m * (sum of squared distances from centroid)
        float CalculateLocalInertia(float mass) const override
        {
            // Calculate centroid
            vec2 centroid = (vertices[0] + vertices[1] + vertices[2]) / 3.0f;
            
            // Sum squared distances from centroid
            float sumSq = 0.0f;
            for (int i = 0; i < 3; ++i)
            {
                vec2 v = vertices[i] - centroid;
                sumSq += v.x * v.x + v.y * v.y;
            }
            
            return (1.0f / 18.0f) * mass * sumSq;
        }

        // Get rotated vertices for collision detection
        void GetWorldVertices(const vec2 &position, const quat &rotation, vec2 out[3]) const
        {
            for (int i = 0; i < 3; ++i)
            {
                // Rotate vertex
                vec3 rotated = rotation * vec3{vertices[i].x, vertices[i].y, 0.0f};
                out[i] = position + vec2{rotated.x, rotated.y};
            }
        }
    };

    struct Collider2D
    {
        std::unique_ptr<PhysicsShape> shape;
        Collider2D() = default;
        explicit Collider2D(std::unique_ptr<PhysicsShape> s) : shape(std::move(s)) {}
    };
} // namespace physics
