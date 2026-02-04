#pragma once
#include <engine/scene.hpp>
#include <components/circle.hpp>
#include <components/physics/circle.hpp>
#include <components/physics/rigidbody2d.hpp>
#include <engine/input.hpp>
#include <cmath>

class BulletComponent : public Component
{
public:
    Scene *scene = nullptr;
    float lifetime = 3.0f;
    float lifetime_remaining = 0.0f;

    void Start() override
    {
        transform = entity->GetComponent<Transform>();
        lifetime_remaining = lifetime;
    }

    void Update(const float &deltaTime) override
    {
        lifetime_remaining -= deltaTime;
        if (lifetime_remaining <= 0.0f && scene)
        {
            scene->Destroy(entity->GetID());
        }

        if (!transform) return;
        
        // Wrap around screen
        if (transform->position.x < 0)
            transform->position.x = 956.0f;
        else if (transform->position.x > 956.0f)
            transform->position.x = 0.0f;

        if (transform->position.y < 0)
            transform->position.y = 540.0f;
        else if (transform->position.y > 540.0f)
            transform->position.y = 0.0f;
    }

private:
    Transform *transform = nullptr;
};

class Spaceship : public Component
{
public:
    Spaceship(Scene *scene) : scene(scene) {}

    void Start() override
    {
        transform = entity->GetComponent<Transform>();
        rigidbody = entity->GetComponent<::Rigidbody2D>();

        // Visual: small circle
        entity->AddComponent<Circle>(8.0f, 3, GREEN);

        // Physics
        if (!rigidbody)
        {
            rigidbody = entity->AddComponent<::Rigidbody2D>();
            rigidbody->mass = 1.0f;
        }

        auto collider = entity->AddComponent<CircleCollider2D>();
        collider->radius = 8.0f;
    }

    void Update(const float &deltaTime) override
    {
        if (!transform || !rigidbody) return;

        auto &input = InputManager::Instance();

        // WASD Movement (direct velocity control)
        vec2 move_direction = {0.0f, 0.0f};
        
        if (input.IsKeyPressed(SDL_SCANCODE_W))
            move_direction.y += 1.0f;
        if (input.IsKeyPressed(SDL_SCANCODE_S))
            move_direction.y -= 1.0f;
        if (input.IsKeyPressed(SDL_SCANCODE_A))
            move_direction.x -= 1.0f;
        if (input.IsKeyPressed(SDL_SCANCODE_D))
            move_direction.x += 1.0f;

        // Normalize direction
        float move_len = std::sqrt(move_direction.x * move_direction.x + move_direction.y * move_direction.y);
        if (move_len > 0.001f)
        {
            move_direction = move_direction / move_len;
        }

        // Get physics body and set velocity
        auto rb = rigidbody->body;
        if (rb)
        {
            rb->velocity = move_direction * move_speed;
        }

        // Mouse aiming - get mouse position and aim towards it
        float screen_y = input.GetMouseY();
        vec2 mouse_pos = {(float)input.GetMouseX(), 540.0f - screen_y};  // Convert Y coordinate
        
        // Calculate direction from player to mouse
        vec2 to_mouse = mouse_pos - vec2(transform->position.x, transform->position.y);
        float dist_to_mouse = std::sqrt(to_mouse.x * to_mouse.x + to_mouse.y * to_mouse.y);
        
        if (dist_to_mouse > 0.001f)
        {
            to_mouse = to_mouse / dist_to_mouse;
            
            // Rotate ship to face mouse
            float angle = std::atan2(to_mouse.x, to_mouse.y);
            transform->rotation = quat::FromAxisAngle(vec3{0.0f, 0.0f, 1.0f}, angle);
        }

        // Wrap around screen
        if (transform->position.x < 0)
            transform->position.x = 956.0f;
        else if (transform->position.x > 956.0f)
            transform->position.x = 0.0f;

        if (transform->position.y < 0)
            transform->position.y = 540.0f;
        else if (transform->position.y > 540.0f)
            transform->position.y = 0.0f;

        // Shooting with left mouse button
        if (input.IsMouseButtonPressed(SDL_BUTTON_LEFT))
        {
            Shoot(to_mouse);
        }
    }

    void Shoot(vec2 direction)
    {
        auto bullet = scene->Create("Bullet#");
        
        if (auto t = bullet->GetComponent<Transform>())
        {
            t->position = transform->position + vec3{direction.x * 15.0f, direction.y * 15.0f, 0.0001f};
        }

        auto rb = bullet->AddComponent<::Rigidbody2D>();
        rb->mass = 0.1f;

        // Get physics body and set velocity
        if (auto rb_physics = rb->body)
        {
            rb_physics->velocity = direction * bullet_speed;
        }

        bullet->AddComponent<Circle>(2.0f, 8, BLUE);

        auto collider = bullet->AddComponent<CircleCollider2D>();
        collider->radius = 2.0f;

        // Add bullet lifetime component
        auto bullet_comp = bullet->AddComponent<BulletComponent>();
        bullet_comp->scene = scene;
        
        // Track bullet in game state (if accessible)
        // This is set in main.cpp
    }

    float move_speed = 250.0f;      // pixels per second
    float bullet_speed = 500.0f;    // pixels per second

private:
    Scene *scene;
    Transform *transform = nullptr;
    ::Rigidbody2D *rigidbody = nullptr;
};

class CameraFollower : public Component
{
public:
    Entity *target = nullptr;
    float follow_speed = 1.5f;  // seconds to catch up (smooth delay)

    void Start() override
    {
        camera_transform = entity->GetComponent<Transform>();
    }

    void Update(const float &deltaTime) override
    {
        if (!target || !camera_transform) return;

        auto target_transform = target->GetComponent<Transform>();
        if (!target_transform) return;

        // Smooth camera follow with delay
        vec3 target_pos = target_transform->position;
        target_pos.z = 0.0f;  // Keep Z at 0

        // Interpolate position smoothly
        float t = 1.0f - std::exp(-5.0f * deltaTime / follow_speed);
        camera_transform->position = camera_transform->position + (target_pos - camera_transform->position) * t;
    }

private:
    Transform *camera_transform = nullptr;
};
