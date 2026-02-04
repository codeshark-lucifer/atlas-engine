#pragma once
#include <SDL3/SDL.h>
#include <ecs/ecs.hpp>
#include <engine/input.hpp>

#include <systems/render.hpp>
#include <systems/physics.hpp>

class Scene
{
public:
    Scene(SDL_Window *win) : window(win) {};
    void Initialize(const int &width, const int &height, const char *name)
    {
        this->width = width;
        this->height = height;
        this->name = name;

        InputManager::Instance().Initialize(window);
        render.Initialize(width, height);
    }

    void Start()
    {
        render.OnStart(world);
        physics.OnStart(world);

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
        InputManager::Instance().Update();
        // physics update
        physics.OnUpdate(world, deltaTime);
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

    void ClearColor(Color color) {
        render.SetBGColor(color);
    }

private:
    const char *name = "";
    int width = 0, height = 0;
    World world;
    SDL_Window *window = nullptr;

    RenderSystem render;
    PhysicsSystem physics;
};