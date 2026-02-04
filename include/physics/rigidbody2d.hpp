#pragma once
#include <ecs/ecs.hpp>
#include "collider2d.hpp"
#include "comp.h"

namespace physics
{
    struct Rigidbody2D
    {
        Rigidbody2D(float mass, const MotionState &motion, PhysicsShape *s)
            : position(motion.position),
              quaternion(motion.quaternion),
              mass(mass),
              shape(s),
              velocity{0.0f, 0.0f},
              angularVelocity(0.0f)
        {
            if (shape)
            {
                inertia = shape->CalculateLocalInertia(mass);
            }
            else
            {
                inertia = 0.0f;
            }

            invMass = (mass > 0.0f) ? 1.0f / mass : 0.0f;
            invInertia = (inertia > 0.0f) ? 1.0f / inertia : 0.0f;
        }

        vec2 position;
        quat quaternion;

        vec2 velocity;
        float angularVelocity = 0.0f;

        vec2 gravity;
        float mass;
        float inertia = 0.0f;   // rotational inertia (scalar)
        float invMass = 0.0f;
        float invInertia = 0.0f;

        PhysicsShape *shape = nullptr; // non-owning

        CollistionObject flag = CollistionObject::None;

        MotionState GetMotion() const
        {
            return MotionState{position, quaternion};
        }
    };

};// namespace physics