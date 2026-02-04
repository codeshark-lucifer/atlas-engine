#pragma once
#include <engine/scene.hpp>
#include <engine/input.hpp>
#include <components/asteroid.hpp>
#include <components/spaceship.hpp>
#include <map>
#include <string>

class CollisionHandler : public Component
{
public:
    CollisionHandler(Scene *scene) : scene(scene) {}

    void Start() override
    {
        // Initialize tracking
    }

    void Update(const float &deltaTime) override
    {
        // Get all entities and check for collisions manually
        // since we need game-specific collision logic
    }

    Scene *scene;
};

class GameUI : public Component
{
public:
    GameUI(Scene *scene, GameManager *game_mgr) : scene(scene), game_manager(game_mgr) {}

    void Start() override {}

    void Update(const float &deltaTime) override
    {
        // UI is typically rendered in render system
        // This could be used for game state logic
    }

private:
    Scene *scene;
    GameManager *game_manager;
};
