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

        void addRigidBody(Rigidbody2D *rigid)
        {
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

            for (auto body : bodies)
            {
                if (!body || body->invMass == 0.0f)
                    continue; // static or kinematic

                // Apply gravity as a FORCE
                body->AddForce(body->gravity * body->mass);

                // Linear integration
                vec2 acceleration = body->force * body->invMass;
                body->velocity += acceleration * deltaTime;
                
                // Apply linear damping (friction)
                float dampingFactor = 1.0f - body->linearDamping * deltaTime;
                body->velocity.x *= dampingFactor;
                body->velocity.y *= dampingFactor;
                
                // Clamp velocity to max velocity
                float velocityMagnitude = sqrt(body->velocity.x * body->velocity.x + body->velocity.y * body->velocity.y);
                if (velocityMagnitude > body->maxVelocity)
                {
                    body->velocity.x = (body->velocity.x / velocityMagnitude) * body->maxVelocity;
                    body->velocity.y = (body->velocity.y / velocityMagnitude) * body->maxVelocity;
                }
                
                body->position += body->velocity * deltaTime;

                // Angular integration
                if (body->angularVelocity != 0.0f && body->invInertia > 0.0f)
                {
                    body->angularVelocity += body->torque * body->invInertia * deltaTime;

                    // Apply angular damping (friction)
                    body->angularVelocity *= (1.0f - body->angularDamping * deltaTime);

                    float angle = body->angularVelocity * deltaTime;
                    quat dq = quat::FromAxisAngle(vec3{0, 0, 1}, angle);
                    body->quaternion = dq * body->quaternion;
                }

                body->ClearForces();
            }

            // --- COLLISIONS (unchanged) ---
            for (size_t i = 0; i < bodies.size(); ++i)
            {
                for (size_t j = i + 1; j < bodies.size(); ++j)
                {
                    Rigidbody2D *bodyA = bodies[i];
                    Rigidbody2D *bodyB = bodies[j];
                    if (!bodyA || !bodyB)
                        continue;

                    bool aStatic = bodyA->invMass == 0.0f;
                    bool bStatic = bodyB->invMass == 0.0f;
                    if (aStatic && bStatic)
                        continue;

                    auto *circleA = bodyA->shape ? dynamic_cast<ShapeCircle *>(bodyA->shape) : nullptr;
                    auto *boxA = bodyA->shape ? dynamic_cast<ShapeBox *>(bodyA->shape) : nullptr;
                    auto *triA = bodyA->shape ? dynamic_cast<ShapeTriangle *>(bodyA->shape) : nullptr;

                    auto *circleB = bodyB->shape ? dynamic_cast<ShapeCircle *>(bodyB->shape) : nullptr;
                    auto *boxB = bodyB->shape ? dynamic_cast<ShapeBox *>(bodyB->shape) : nullptr;
                    auto *triB = bodyB->shape ? dynamic_cast<ShapeTriangle *>(bodyB->shape) : nullptr;

                    if (circleA && boxB)
                        ResolveCircleBoxCollision(bodyA, circleA, bodyB, boxB, 0.8f);
                    else if (boxA && circleB)
                        ResolveCircleBoxCollision(bodyB, circleB, bodyA, boxA, 0.8f);
                    else if (circleA && circleB)
                        ResolveCircleCircleCollision(bodyA, circleA, bodyB, circleB, 0.8f);
                    else if (triA && circleB)
                        ResolveCircleTriangleCollision(bodyB, circleB, bodyA, triA, 0.8f);
                    else if (triB && circleA)
                        ResolveCircleTriangleCollision(bodyA, circleA, bodyB, triB, 0.8f);
                }
            }
        }

        void ResolveCircleBoxCollision(Rigidbody2D *circle, physics::ShapeCircle *circleShape,
                                       Rigidbody2D *box, physics::ShapeBox *boxShape, float restitution)
        {
            if (!circleShape || !boxShape)
                return;

            // OBB (Oriented Bounding Box) collision: account for box rotation
            // Get box's local axes from its quaternion
            vec3 boxAxisX = box->quaternion * vec3{1.0f, 0.0f, 0.0f};
            vec3 boxAxisY = box->quaternion * vec3{0.0f, 1.0f, 0.0f};

            // Find closest point on rotated box to circle
            vec2 diff = circle->position - box->position;

            // Project diff onto box's local axes
            float distX = diff.x * boxAxisX.x + diff.y * boxAxisX.y;
            float distY = diff.x * boxAxisY.x + diff.y * boxAxisY.y;

            // Clamp to box half-extents
            distX = std::max(-boxShape->halfSize.x, std::min(distX, boxShape->halfSize.x));
            distY = std::max(-boxShape->halfSize.y, std::min(distY, boxShape->halfSize.y));

            // Convert back to world space
            vec2 closest = box->position + vec2{
                                               distX * boxAxisX.x + distY * boxAxisY.x,
                                               distX * boxAxisX.y + distY * boxAxisY.y};

            vec2 normal = circle->position - closest;
            float dist = std::sqrt(normal.x * normal.x + normal.y * normal.y);
            float penetration = circleShape->radius - dist;

            if (penetration > 0.0f)
            {
                // Normalize collision normal
                normal = dist > 0.001f ? normal / dist : vec2{0.0f, 1.0f};

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
            float minDist = shapeA->radius + shapeB->radius;
            float penetration = minDist - dist;

            if (penetration > 0.0f)
            {
                // Normal points from A to B
                vec2 normal = dist > 0.001f ? diff / dist : vec2{0.0f, 1.0f};

                // Separate bodies to prevent stickiness
                const float slop = 0.01f; // Separation threshold
                float totalMass = circleA->mass + circleB->mass;

                if (circleA->mass > 0.0f && circleB->mass == 0.0f)
                {
                    // A is dynamic, B is static: push A away
                    circleA->position += normal * (-penetration - slop);
                }
                else if (circleA->mass == 0.0f && circleB->mass > 0.0f)
                {
                    // A is static, B is dynamic: push B away
                    circleB->position += normal * (penetration + slop);
                }
                else if (circleA->mass > 0.0f && circleB->mass > 0.0f)
                {
                    // Both dynamic: push both away proportionally to mass
                    float ratioA = circleB->mass / totalMass;
                    float ratioB = circleA->mass / totalMass;
                    circleA->position += normal * (-(penetration + slop) * ratioA);
                    circleB->position += normal * ((penetration + slop) * ratioB);
                }

                // Resolve velocity (impulse-based)
                vec2 relVel = circleA->velocity - circleB->velocity;
                float velAlongNormal = relVel.x * normal.x + relVel.y * normal.y;

                // Only resolve if velocities are moving towards each other
                if (velAlongNormal < 0.0f)
                {
                    // Use effective restitution (reduce bounciness to prevent stickiness)
                    float e = restitution * 0.6f; // Dampen restitution
                    float denominator = circleA->invMass + circleB->invMass;

                    if (denominator > 0.0f)
                    {
                        float impulse = -(1.0f + e) * velAlongNormal / denominator;
                        circleA->velocity += normal * (impulse * circleA->invMass);
                        circleB->velocity += normal * (-impulse * circleB->invMass);
                    }
                }
            }
        }

        void ResolveCircleTriangleCollision(Rigidbody2D *circle, physics::ShapeCircle *circleShape,
                                            Rigidbody2D *tri, physics::ShapeTriangle *triShape, float restitution)
        {
            if (!circleShape || !triShape)
                return;

            // Get triangle vertices in world space
            vec2 triVerts[3];
            triShape->GetWorldVertices(tri->position, tri->quaternion, triVerts);

            // Find closest point on triangle to circle
            vec2 closest = triVerts[0];
            float closestDist = 1e9f;

            // Check against all three edges
            for (int i = 0; i < 3; ++i)
            {
                vec2 v0 = triVerts[i];
                vec2 v1 = triVerts[(i + 1) % 3];
                vec2 edge = v1 - v0;
                vec2 diff = circle->position - v0;

                // Project circle onto edge
                float edgeLenSq = edge.x * edge.x + edge.y * edge.y;
                float t = (diff.x * edge.x + diff.y * edge.y) / edgeLenSq;
                t = std::max(0.0f, std::min(1.0f, t));

                vec2 point = v0 + edge * t;
                vec2 toDist = circle->position - point;
                float dist = std::sqrt(toDist.x * toDist.x + toDist.y * toDist.y);

                if (dist < closestDist)
                {
                    closestDist = dist;
                    closest = point;
                }
            }

            float penetration = circleShape->radius - closestDist;

            if (penetration > 0.0f)
            {
                vec2 normal = circle->position - closest;
                float dist = std::sqrt(normal.x * normal.x + normal.y * normal.y);
                normal = dist > 0.001f ? normal / dist : vec2{0.0f, 1.0f};

                // Separate bodies
                if (circle->mass > 0.0f && tri->mass == 0.0f)
                {
                    circle->position += normal * (penetration + 0.01f);
                }
                else if (circle->mass > 0.0f && tri->mass > 0.0f)
                {
                    circle->position += normal * (penetration * 0.5f + 0.01f);
                    tri->position += normal * (-penetration * 0.5f - 0.01f);
                }

                // Resolve velocity
                float relVel = (circle->velocity.x - tri->velocity.x) * normal.x +
                               (circle->velocity.y - tri->velocity.y) * normal.y;

                if (relVel < 0.0f)
                {
                    float denominator = circle->invMass + tri->invMass;
                    if (denominator > 0.0f)
                    {
                        float impulse = -(1.0f + restitution) * relVel / denominator;
                        circle->velocity += normal * impulse * circle->invMass;
                        tri->velocity += normal * (-impulse * tri->invMass);
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
        std::vector<Rigidbody2D *> bodies;
    };
};