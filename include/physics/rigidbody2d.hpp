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
              angularVelocity(0.0f),
              force{0.0f, 0.0f},
              torque(0.0f)
        {
            if (shape)
                inertia = shape->CalculateLocalInertia(mass);

            invMass = (mass > 0.0f) ? 1.0f / mass : 0.0f;
            invInertia = (inertia > 0.0f) ? 1.0f / inertia : 0.0f;
        }

        vec2 gravity;

        // --- TRANSFORM ---
        vec2 position;
        quat quaternion;

        // --- VELOCITY ---
        vec2 velocity;
        float angularVelocity;

        // --- FRICTION/DAMPING ---
        float linearDamping = 0.5f;  // Friction for linear velocity (increased for faster stopping)
        float angularDamping = 0.1f; // Friction for angular velocity
        
        // --- VELOCITY LIMITS ---
        float maxVelocity = 50.0f;  // Maximum velocity magnitude

        // --- FORCE ACCUMULATORS ---
        vec2 force;
        float torque;

        // --- MASS ---
        float mass;
        float inertia = 0.0f;
        float invMass = 0.0f;
        float invInertia = 0.0f;

        PhysicsShape *shape = nullptr;
        CollistionObject flag = CollistionObject::None;

        // --- API ---
        void AddForce(const vec2 &f)
        {
            if (invMass == 0.0f)
                return;
            force += f;
        }

        void AddImpulse(const vec2 &impulse)
        {
            if (invMass == 0.0f)
                return;
            velocity += impulse * invMass;
        }

        void ClearForces()
        {
            force = {0.0f, 0.0f};
            torque = 0.0f;
        }

        MotionState GetMotion() const
        {
            return MotionState{position, quaternion};
        }
    };
}
