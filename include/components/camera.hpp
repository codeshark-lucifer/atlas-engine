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

    void Start() override {
        transform = entity->GetComponent<Transform>();
    }

    void SetUniform(Shader &shader)
    {
        // Use a bottom = 0, top = height orthographic projection so
        // positive Y points UP in world coordinates (W increases Y -> up)
        mat4 projection = mat4::Ortho(0, width, 0, height, -1, 999);
        // The view should be the inverse of the camera transform
        mat4 view = mat4::Translate(-transform->position);

        shader.Use();
        shader.SetUniform("projection", projection);
        shader.SetUniform("view", view);
    }

private:
    Transform *transform = nullptr;
    int width = 0, height = 0;
};