#pragma once
#include <physics/comp.h>
#include <physics/rigidbody2d.hpp>

class Collider2D : public PhysicsComponent
{
public:
    virtual physics::PhysicsShape* CreateShape() = 0;
    virtual ~Collider2D() = default;
};