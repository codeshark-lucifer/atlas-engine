#pragma once
#include <engine/scene.hpp>
#include <components/circle.hpp>
#include <components/physics/circle.hpp>
#include <components/physics/rigidbody2d.hpp>
#include <cmath>
#include <random>

class Asteroid : public Component
{
public:
    enum Size { LARGE, MEDIUM, SMALL };

    Asteroid(Scene *scene, Size size = LARGE) 
        : scene(scene), size(size), age(0.0f)
    {
    }

    void Start() override
    {
        transform = entity->GetComponent<Transform>();
        
        // Set size properties
        float radius, mass;
        switch (size)
        {
            case LARGE:
                radius = 30.0f;
                mass = 2.0f;
                score_value = 20;
                break;
            case MEDIUM:
                radius = 15.0f;
                mass = 1.0f;
                score_value = 50;
                break;
            case SMALL:
                radius = 7.0f;
                mass = 0.5f;
                score_value = 100;
                break;
        }

        // Add visual representation
        entity->AddComponent<Circle>(radius, 16, GREEN);

        // Add physics
        auto rb = entity->AddComponent<::Rigidbody2D>();
        rb->mass = mass;
        auto collider = entity->AddComponent<CircleCollider2D>();
        collider->radius = radius;

        // Random rotation
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(-360.0f, 360.0f);
        
        auto rb_physics = rb->body;
        if (rb_physics)
        {
            // Rotation in degrees per second
            rotation_speed = dis(gen);
        }
    }

    void Update(const float &deltaTime) override
    {
        age += deltaTime;

        // Rotate asteroid
        if (transform)
        {
            float angle = rotation_speed * age;
            float rad = angle * 3.1415926f / 180.0f;
            transform->rotation = quat::FromAxisAngle(vec3{0.0f, 0.0f, 1.0f}, rad);
        }
    }

    bool IsOffScreen(int width, int height)
    {
        if (!transform) return false;
        float x = transform->position.x;
        float y = transform->position.y;
        // Destroy if too far off screen
        return x < -200 || x > width + 200 || y < -200 || y > height + 200;
    }

    Size GetSize() const { return size; }
    int GetScoreValue() const { return score_value; }

private:
    Scene *scene;
    Transform *transform = nullptr;
    Size size;
    float rotation_speed = 0.0f;
    float age = 0.0f;
    int score_value = 0;
};
