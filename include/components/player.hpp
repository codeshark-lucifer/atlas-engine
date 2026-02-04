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
        rb = entity->GetComponent<Rigidbody2D>();

        if (rb)
        {
            rb->mass = 1.0f;
            rb->useGravity = false;
            rb->gravity = {0.0f, 0.0f};
        }

        auto collider = entity->GetComponent<CircleCollider2D>();
        if (collider)
        {
            collider->radius = radius;
        }

        // Add render circle
        entity->AddComponent<Circle>(radius, 32, WHITE, std::make_shared<Texture2D>("assets/textures/cup.png"));

        debuger = scene->Create("Debugger");
        debuger->AddComponent<Circle>(5.0f, 32, RED);
    }

    void Update(const float &deltaTime) override
    {
        auto &input = InputManager::Instance();
        vec2 move = input.GetMoveDirection();

        // The physics body might not be created until after the first frame
        // due to PhysicsSystem initialization
        if (rb && rb->body && vec2::Dot(move, move) > 0.1f)
        {
            rb->body->AddForce(move * speed);
        }

        // Make player face towards mouse
        if (transform)
        {
            float mouseX = input.GetMouseX();
            float mouseY = 540.0f - input.GetMouseY();

            // Calculate direction from player to mouse
            float dirX = mouseX - transform->position.x;
            float dirY = mouseY - transform->position.y;
            
            // Calculate angle (atan2 returns angle in radians)
            float angle = atan2f(dirY, dirX);
            
            // Create quaternion from angle (rotating around Z axis)
            transform->rotation = quat::FromAxisAngle(vec3{0, 0, 1}, angle);

            debuger->GetComponent<Transform>()->position = {mouseX, mouseY, 0.0f};
        }
    }

private:
    float speed = 200.0f;
    float radius = 15.0f;
    Scene *scene = nullptr;
    Transform *transform = nullptr;
    Rigidbody2D *rb = nullptr;
    Entity *debuger = nullptr;
};