#pragma once
#include <ecs/system.hpp>

class EngineSystem : public System
{
public:
    EngineSystem() : System("EngineSystem") {}

    void Initialize(const int &width, const int &height)
    {
        this->width = width;
        this->height = height;
    }

    void Start(const EntityMap &entities) override
    {
        {
            // render start here first
            // ...
        }
        for (const auto &[id, entity] : entities)
        {
            for (const auto &comp : entity->GetAllComponent())
            {
                comp->Start();
            }
        }
    }

    void Update(const EntityMap &entities, const float &deltaTime) override
    {
        for (const auto &[id, entity] : entities)
        {
            for (const auto &comp : entity->GetAllComponent())
            {
                comp->Update(deltaTime);
            }
        }
    }

    void Render(const EntityMap &entities) override
    {
        glViewport(0, 0, width, height);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Resize(const EntityMap &entities, int w, int h) override
    {
        this->width = width;
        this->height = height;
    }

    void DrawUI(const EntityMap &entities) override
    {
        for (const auto &[id, entity] : entities)
        {
            for (const auto &comp : entity->GetAllComponent())
            {
                comp->DrawUI();
            }
        }
    }

private:
    int width = 0, height = 0;
};