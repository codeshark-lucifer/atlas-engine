#pragma once
#include <engine/scene.hpp>
#include <components/circle.hpp>
#include <iostream>

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
        auto sprite = entity->AddComponent<Circle>(100.0f);
    }

    void Update(const float &deltaTime) override
    {
        auto &input = InputManager::Instance();
        float speed = 200.0f;
        if(input.IsKeyPressed(SDL_SCANCODE_W))
            transform->position.y -= speed * deltaTime;
        if(input.IsKeyPressed(SDL_SCANCODE_S))
            transform->position.y += speed * deltaTime;
        if(input.IsKeyPressed(SDL_SCANCODE_A))
            transform->position.x -= speed * deltaTime;
        if(input.IsKeyPressed(SDL_SCANCODE_D))
            transform->position.x += speed * deltaTime;
        
    }

private:
    Scene* scene = nullptr;
    Transform* transform = nullptr;
};