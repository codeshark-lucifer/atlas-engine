#pragma once
#include <ecs/ecs.hpp>
#include <vector>
#include <algorithm>
#include "rigidbody2d.hpp"
#include "collider2d.hpp"
#include <iostream>

namespace physics
{
    class PhysicsWorld
    {
    public:
        PhysicsWorld() {}
        ~PhysicsWorld() { Clean(); }

        void setGravity(vec2 g) { gravity = g; }
        void addRigidBody(Rigidbody2D *rigid)
        {
            std::cout << "ADD RIGID\n";
            if (std::find(bodies.begin(), bodies.end(), rigid) == bodies.end())
                bodies.push_back(rigid);
        }

        void removeRigidBody(Rigidbody2D *rigid)
        {
            auto it = std::find(bodies.begin(), bodies.end(), rigid);
            if (it != bodies.end())
            {
                bodies.erase(it); // Erase the found element
                delete rigid;
            }
            else
            {
                throw std::runtime_error("This body is not found in bodies!");
            }
        }

        void step(const float &deltaTime)
        {
            if (deltaTime <= 0.0f)
                return;

            // Apply forces and integrate
            for (auto body : bodies)
            {
                if (!body)
                    continue;

                // Apply gravity only to dynamic bodies
                if (body->mass > 0.0f)
                {
                    // integrate linear velocity
                    body->velocity += gravity * deltaTime;

                    // integrate position
                    body->position += body->velocity * deltaTime;

                    // integrate rotation (simple Euler)
                    if (body->angularVelocity != 0.0f)
                    {
                        float angle = body->angularVelocity * deltaTime;
                        quat dq = quat::FromAxisAngle(vec3{0.0f, 0.0f, 1.0f}, angle);
                        body->quaternion = dq * body->quaternion;
                    }
                }
            }

            // Simple collision detection and response (circle-box AABB)
            for (size_t i = 0; i < bodies.size(); ++i)
            {
                for (size_t j = i + 1; j < bodies.size(); ++j)
                {
                    Rigidbody2D *bodyA = bodies[i];
                    Rigidbody2D *bodyB = bodies[j];
                    if (!bodyA || !bodyB)
                        continue;

                    // Skip if both are static
                    bool aStatic = bodyA->mass == 0.0f;
                    bool bStatic = bodyB->mass == 0.0f;
                    if (aStatic && bStatic)
                        continue;

                    // Simple AABB collision for boxes and circles
                    physics::ShapeCircle *circleA = bodyA->shape ? dynamic_cast<physics::ShapeCircle *>(bodyA->shape) : nullptr;
                    physics::ShapeBox *boxA = bodyA->shape ? dynamic_cast<physics::ShapeBox *>(bodyA->shape) : nullptr;
                    physics::ShapeCircle *circleB = bodyB->shape ? dynamic_cast<physics::ShapeCircle *>(bodyB->shape) : nullptr;
                    physics::ShapeBox *boxB = bodyB->shape ? dynamic_cast<physics::ShapeBox *>(bodyB->shape) : nullptr;

                    // Circle-Box collision
                    if (circleA && boxB)
                        ResolveCircleBoxCollision(bodyA, circleA, bodyB, boxB, 0.8f);
                    else if (boxA && circleB)
                        ResolveCircleBoxCollision(bodyB, circleB, bodyA, boxA, 0.8f);
                    // Circle-Circle collision
                    else if (circleA && circleB)
                        ResolveCircleCircleCollision(bodyA, circleA, bodyB, circleB, 0.8f);
                }
            }
        }

        void ResolveCircleBoxCollision(Rigidbody2D *circle, physics::ShapeCircle *circleShape,
                                       Rigidbody2D *box, physics::ShapeBox *boxShape, float restitution)
        {
            if (!circleShape || !boxShape)
                return;
            // Simple AABB check: circle center vs box
            vec2 boxMin = box->position - boxShape->halfSize;
            vec2 boxMax = box->position + boxShape->halfSize;
            vec2 closest = circle->position;
            closest.x = std::max(boxMin.x, std::min(closest.x, boxMax.x));
            closest.y = std::max(boxMin.y, std::min(closest.y, boxMax.y));

            vec2 diff = circle->position - closest;
            float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
            float penetration = circleShape->radius - dist;

            if (penetration > 0.0f)
            {
                // Collision detected
                vec2 normal = dist > 0.001f ? diff / dist : vec2{0.0f, 1.0f};

                // Separate bodies
                if (circle->mass > 0.0f && box->mass == 0.0f)
                {
                    circle->position += normal * (penetration + 0.01f);
                }
                else if (circle->mass > 0.0f && box->mass > 0.0f)
                {
                    circle->position += normal * (penetration * 0.5f + 0.01f);
                    box->position += normal * (-penetration * 0.5f - 0.01f);
                }

                // Resolve velocity (impulse-based)
                float relVel = (circle->velocity.x - box->velocity.x) * normal.x +
                               (circle->velocity.y - box->velocity.y) * normal.y;

                if (relVel < 0.0f) // Approaching
                {
                    float denominator = circle->invMass + box->invMass;
                    if (denominator > 0.0f)
                    {
                        float impulse = -(1.0f + restitution) * relVel / denominator;
                        circle->velocity += normal * impulse * circle->invMass;
                        box->velocity += normal * (-impulse * box->invMass);
                    }
                }
            }
        }

        void ResolveCircleCircleCollision(Rigidbody2D *circleA, physics::ShapeCircle *shapeA,
                                          Rigidbody2D *circleB, physics::ShapeCircle *shapeB, float restitution)
        {
            if (!shapeA || !shapeB)
                return;
            vec2 diff = circleB->position - circleA->position;
            float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
            float penetration = (shapeA->radius + shapeB->radius) - dist;

            if (penetration > 0.0f)
            {
                vec2 normal = dist > 0.001f ? diff / dist : vec2{0.0f, 1.0f};

                // Separate bodies
                if (circleA->mass > 0.0f && circleB->mass == 0.0f)
                {
                    circleA->position += normal * (-penetration - 0.01f);
                }
                else if (circleA->mass > 0.0f && circleB->mass > 0.0f)
                {
                    circleA->position += normal * (-penetration * 0.5f - 0.01f);
                    circleB->position += normal * (penetration * 0.5f + 0.01f);
                }

                // Resolve velocity (impulse-based)
                float relVel = (circleA->velocity.x - circleB->velocity.x) * normal.x +
                               (circleA->velocity.y - circleB->velocity.y) * normal.y;

                if (relVel < 0.0f)
                {
                    float denominator = circleA->invMass + circleB->invMass;
                    if (denominator > 0.0f)
                    {
                        float impulse = -(1.0f + restitution) * relVel / denominator;
                        circleA->velocity += normal * impulse * circleA->invMass;
                        circleB->velocity += normal * (-impulse * circleB->invMass);
                    }
                }
            }
        }

        void Clean()
        {
            for (auto body : bodies)
                delete body;

            bodies.clear();
        }

    private:
        vec2 gravity = {0.0f, 0.0f};
        std::vector<Rigidbody2D *> bodies;
    };
};