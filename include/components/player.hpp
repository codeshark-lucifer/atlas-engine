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
        entity->AddComponent<Circle>(5.0f);
    }

    void Update(const float &deltaTime) override
    {
        auto &input = InputManager::Instance();
        // Convert screen coordinates (Y down) to world coordinates (Y up)
        // Screen height is 540, so invert Y: screenY -> (540 - screenY)
        float screenY = input.GetMouseY();
        vec2 mouse = {input.GetMouseX(), 540.0f - screenY};
        transform->position = {mouse.x, mouse.y, 0.0f};
        if(input.IsMouseButtonPressed(SDL_BUTTON_LEFT)) {
            CreateSphere(mouse);
        }
    }

    void CreateSphere(vec2 pos)
    {
        auto ball = scene->Create("Ball#");
        float radius = 10.0f;
        
        // Set position FIRST via Transform
        if (auto t = ball->GetComponent<Transform>())
        {
            t->position = {pos.x, pos.y, 0.0001f};
        }
        
        ball->AddComponent<Circle>(radius);

        // Add collider FIRST so physics system can find it
        auto collider = ball->AddComponent<CircleCollider2D>();
        collider->radius = radius;

        // Then add rigidbody (body will be created by physics system later)
        auto rb = ball->AddComponent<Rigidbody2D>();
        rb->mass = 1.0f; // Dynamic: has mass so it falls under gravity
    }

private:
    Scene *scene = nullptr;
    Transform *transform = nullptr;
};