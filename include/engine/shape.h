#pragma once

#include <engine/texture2D.hpp>
#include <engine/shader.hpp>
#include <engine/color.h>
#include <engine/mesh.hpp>

#include <ecs/ecs.hpp>
#include <memory>
#include <cmath>

class Shape : public Component
{
public:
    ~Shape()
    {
        if (mesh)
            delete mesh;
    }
    virtual void Render(Shader &_shader) {}
    Mesh *mesh = nullptr;
    std::shared_ptr<Texture2D> sprite = nullptr;
    Color color;
    Shader *shader = nullptr;
};