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
    }

    void Update(const float &deltaTime) override
    {
        auto &input = InputManager::Instance();
    }

private:
    Scene *scene = nullptr;
    Transform *transform = nullptr;
};