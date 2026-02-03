#pragma once
#include <engine/shape.h>

class Circle : public Shape
{
public:
    Circle(float radius, int segments = 32, const Color &color = WHITE, std::shared_ptr<Texture2D> sprite = nullptr)
    {
        this->color = color;
        if (sprite)
            this->sprite = sprite;
        else
            this->sprite = std::make_shared<Texture2D>("assets/textures/default_sprite.png");

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        vertices.push_back({{0.0f, 0.0f}, {0.5f, 0.5f}});

        for (int i = 0; i <= segments; i++)
        {
            float angle = (float)i / (float)segments * 2.0f * 3.1415926f;
            float x = std::cos(angle) * radius;
            float y = std::sin(angle) * radius;

            float u = (x / radius + 1.0f) * 0.5f;
            float v = (y / radius + 1.0f) * 0.5f;

            vertices.push_back({{x, y}, {u, v}});
        }

        for (int i = 1; i <= segments; i++)
        {
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i + 1);
        }

        mesh = new Mesh(vertices, indices);
    }

    void Render(Shader &shader)
    {
        shader.Use();
        shader.SetUniform("model", entity->GetComponent<Transform>()->worldMatrix);
        shader.SetUniform("color", color);

        bool diffuseTextureFound = false;
        if (sprite->type == Type::DIFFUSE && !diffuseTextureFound)
        {
            sprite->Bind(0);
            shader.SetUniform("diffuse_texture1", 0);
            diffuseTextureFound = true;
        }

        mesh->Draw();
    }

};
