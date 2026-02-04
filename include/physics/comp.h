#pragma once
#include <ecs/ecs.hpp>
#include "rigidbody2d.hpp"

class PhysicsComponent : public Component
{
public:
    physics::Rigidbody2D *body = nullptr;
};