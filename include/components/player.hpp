#pragma once
#include <iostream>
#include <engine/scene.hpp>
#include <components/circle.hpp>
#include <components/physics/circle.hpp>
#include <components/physics/rigidbody2d.hpp>

class Player : public Component
{
public:
    Player(Scene *scene)
    {
        this->scene = scene;
    }

    ~Player()
    {
    }

    void Start() override
    {
        transform = entity->GetComponent<Transform>();

        float radius = 10.0f;
        entity->AddComponent<Circle>(radius);
        
        // Add collider FIRST so physics system can find it
        auto collider = entity->AddComponent<CircleCollider2D>();
        collider->radius = radius;
        
        // Then add rigidbody
        auto rb = entity->AddComponent<Rigidbody2D>();
        rb->mass = 1.0f;  // Dynamic: has mass so it falls under gravity
    }

    void Update(const float &deltaTime) override
    {
        auto &input = InputManager::Instance();
    }

private:
    Scene *scene = nullptr;
    Transform *transform = nullptr;
};