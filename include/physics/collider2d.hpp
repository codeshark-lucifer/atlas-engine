#pragma once
#include <ecs/ecs.hpp>
#include "comp.h"

namespace physics
{
    struct MotionState
    {
        vec2 position;
        quat quaternion;

        MotionState(vec2 position, quat quaternion)
        {
            this->position = position;
            this->quaternion = quaternion;
        };
    };

    struct PhysicsShape
    {
    };

    struct ShapeBox : public PhysicsShape
    {
        ShapeBox(vec2 halfSize)
        {
        }
    };

    struct ShapeCircle : public PhysicsShape
    {
        ShapeCircle(float radius)
        {
        }
    };

    struct Collider2D
    {
        PhysicsShape shape;
    };
} // namespace physics
