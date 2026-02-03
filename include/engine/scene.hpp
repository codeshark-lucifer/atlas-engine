#pragma once
#include <SDL3/SDL.h>
#include <ecs/ecs.hpp>
#include <systems/render.hpp>

class Scene
{
public:
    Scene(SDL_Window *win) : window(win) {};
    void Initialize(const int &width, const int &height, const char *name)
    {
        this->width = width;
        this->height = height;
        this->name = name;

        render.Initialize(width, height);
    }

    void Start()
    {
        render.OnStart(world);
        
        for (const auto &[id, entity] : world.GetEntities())
        {
            for (const auto &comp : entity->GetAllComponent())
            {
                comp->Start();
            }
        }
    }

    void Update(const float &deltaTime)
    {
        UpdateTransforms(world, deltaTime);
        // input
        // update
        for (const auto &[id, entity] : world.GetEntities())
        {
            for (const auto &comp : entity->GetAllComponent())
            {
                comp->Update(deltaTime);
            }
        }
        // render
        render.OnRender(world);
    }

    Entity *Create(const char *name)
    {
        return CreateEntity(name, world);
    }

    void Destroy(const EntityID &id)
    {
        world.RemoveEntity(id);
    }

private:
    const char *name = "";
    int width = 0, height = 0;
    World world;
    SDL_Window *window = nullptr;

    RenderSystem render;
};