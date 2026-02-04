#pragma once
#include <physics/comp.h>
#include <physics/rigidbody2d.hpp>

class Rigidbody2D : public PhysicsComponent
{
public:
    float mass{0.0f};
    bool isKinematic{false};
    bool useGravity{true};
};