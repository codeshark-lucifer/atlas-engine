#pragma once
#include <ecs/ecs.hpp>

namespace physics { struct Rigidbody2D; struct PhysicsShape; }

class PhysicsComponent : public Component
{
public:
    physics::Rigidbody2D *body = nullptr;
    physics::PhysicsShape *shape = nullptr;
};