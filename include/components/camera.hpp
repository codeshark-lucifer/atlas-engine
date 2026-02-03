#pragma once
#include <ecs/ecs.hpp>
#include <engine/shader.hpp>

class Camera : public Component
{
public:
    Camera() { Initialize(956, 540); }
    void Initialize(const int &width, const int &height)
    {
        this->width = width;
        this->height = height;
    }

    void SetUniform(Shader& shader) {
        mat4 projection = mat4::Ortho(0, width, height, 0, -1, 1);
        mat4 view = mat4::Identity();
        view.Translate(entity->GetComponent<Transform>()->position);

        shader.Use();
        shader.SetUniform("projection", projection);
        shader.SetUniform("view", view);
    }

private:
    int width = 0, height = 0;
};