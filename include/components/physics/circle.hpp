#pragma once
#include "collider2d.hpp"

class CircleCollider2D : public Collider2D
{
public:
    float radius = 1.0f;

    physics::PhysicsShape* CreateShape() override
    {
        return new physics::ShapeCircle(radius);
    }
};