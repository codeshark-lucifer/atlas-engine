#pragma once
#include <ecs/system.hpp>
#include <engine/shape.h>
#include <components/camera.hpp>

class RenderSystem : public System
{
public:
    RenderSystem() : System("RenderSystem") {}
    ~RenderSystem()
    {
        if (defaultShader)
            delete defaultShader;
    }

    void Initialize(const int &width, const int &height)
    {
        this->width = width;
        this->height = height;
        defaultShader = new Shader("assets/shaders/scene.glsl");
    }

    void OnStart(World &world) override
    {
        for (const auto &[id, entity] : world.GetEntities())
        {
            for (const auto &comp : entity->GetAllComponent())
            {
                if (auto camera = dynamic_cast<Camera *>(comp))
                    defaultCamera = camera;
            }
        }
    }

    void OnRender(World &world) override
    {
        if (defaultCamera)
            defaultCamera->SetUniform(*defaultShader);
        for (const auto &[id, entity] : world.GetEntities())
        {
            for (const auto &comp : entity->GetAllComponent())
            {
                if (auto shape = dynamic_cast<Shape *>(comp))
                    shape->Render(*defaultShader);
            }
        }
    }

    void OnUI(World &world) override
    {
    }

    void OnResize(World &world, int w, int h) override
    {
    }

private:
    int width = 0, height = 0;
    Shader *defaultShader = nullptr;
    Camera *defaultCamera = nullptr;
};