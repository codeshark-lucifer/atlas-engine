#pragma once
#include <physics/comp.h>
#include <physics/rigidbody2d.hpp>

class Rigidbody2D : public PhysicsComponent
{
public:
    float mass{0.0f};
    bool isKinematic{false};
    bool useGravity{true};

    void Start() override
    {
        Transform *trans = entity->GetComponent<Transform>();
        if (body)
            body->position = {trans->position.x, trans->position.y};
    }
};