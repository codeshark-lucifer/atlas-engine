#pragma once
#include <engine/shape.h>

class React : public Shape
{
public:
    React(vec2 size, const Color &color = WHITE, std::shared_ptr<Texture2D> sprite = nullptr)
    {
        this->color = color;
        if (sprite)
            this->sprite = sprite;
        else
            this->sprite = std::make_shared<Texture2D>("assets/textures/default_sprite.png");

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

    void Render(Shader &shader) override
    {
        shader.Use();
        shader.SetUniform("model", entity->GetComponent<Transform>()->worldMatrix);
        shader.SetUniform("color", color);
        bool diffuseTextureFound = false;
        if (sprite->type == Type::DIFFUSE && !diffuseTextureFound)
        {
            sprite->Bind(0); // Bind first diffuse texture to unit 0
            shader.SetUniform("diffuse_texture1", 0);
            diffuseTextureFound = true;
        }
        else if (sprite->type == Type::SPECULAR)
        {
            // For now, ignoring specular textures as the shader doesn't handle them
            // correctly with multiple units. The fragment shader only has diffuse_texture1.
            // If specular textures are needed, the fragment shader needs to be updated
            // to declare more samplers (e.g., specular_texture1, etc.)
        }
        mesh->Draw();
    }

};