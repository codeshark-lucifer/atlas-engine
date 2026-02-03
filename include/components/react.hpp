#pragma once
#include <engine/shape.h>

class React : public Shape
{
public:
    React(vec2 size, const Color &color = WHITE, Shader *shader = nullptr, std::shared_ptr<Texture2D> sprite = nullptr)
    {
        this->shader = shader;
        this->color = color;
        this->sprite = sprite;

        float hw = size.x * 0.5f;
        float hh = size.y * 0.5f;

        std::vector<Vertex> vertices = {
            {{-hw, -hh}, {0.0f, 0.0f}},
            {{hw, -hh}, {1.0f, 0.0f}},
            {{hw, hh}, {1.0f, 1.0f}},
            {{-hw, hh}, {0.0f, 1.0f}},
        };

        std::vector<unsigned int> indices = {
            0, 1, 2,
            2, 3, 0};

        mesh = new Mesh(vertices, indices);
    }

    void Render(Shader &_shader) override
    {
        auto sh = shader == nullptr ? _shader : *shader;
        sh.Use();
        sh.SetUniform("model", entity->GetComponent<Transform>()->worldMatrix);
        sh.SetUniform("color", color);
        if (sprite)
        {
            sh.SetUniform("has_texture", true);
            sprite->Bind(0);
            sh.SetUniform("diffuse_texture1", 0);
        }
        else
        {
            sh.SetUniform("has_texture", false);
        }
        mesh->Draw();
    }

};