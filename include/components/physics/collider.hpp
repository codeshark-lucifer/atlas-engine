#pragma once
#include <physics/comp.h>
#include <physics/rigidbody2d.hpp>

class Collider : public PhysicsComponent
{
public:
    physics::PhysicsShape shape;
};