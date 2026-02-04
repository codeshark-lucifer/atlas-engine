#pragma once
#include <physics/comp.h>
#include <physics/rigidbody2d.hpp>

class Rigidbody2D : public PhysicsComponent
{
public:
    float mass{0.0f};
    bool isKinematic{false};
    bool useGravity{true};
    vec2 gravity{0.0f};
    
    // Friction/Damping properties
    float linearDamping{0.5f};  // Controls how quickly velocity decreases (higher = stops faster)
    float angularDamping{0.1f}; // Controls how quickly rotation slows down
    
    // Velocity limits
    float maxVelocity{300.0f};   // Maximum velocity magnitude

    void Start() override
    {
        Transform *trans = entity->GetComponent<Transform>();
        if (body)
        {
            body->position = {trans->position.x, trans->position.y};
            body->gravity = gravity;
            body->linearDamping = linearDamping;
            body->angularDamping = angularDamping;
            body->maxVelocity = maxVelocity;
        }
    }
};