#pragma once
#include "collider2d.hpp"

class BoxCollider2D : public Collider2D
{
public:
    vec2 size = vec2(1.0f);

    physics::PhysicsShape* CreateShape() override
    {
        return new physics::ShapeBox(size * 0.5f);
    }
};