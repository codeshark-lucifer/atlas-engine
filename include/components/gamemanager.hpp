#pragma once
#include <engine/scene.hpp>
#include <engine/input.hpp>
#include <components/asteroid.hpp>
#include <components/spaceship.hpp>
#include <iostream>
#include <random>
#include <sstream>
#include <iomanip>

class GameManager : public Component
{
public:
    GameManager(Scene *scene) : scene(scene) {}

    void Start() override
    {
        std::random_device rd;
        generator.seed(rd());

        // Spawn initial asteroids
        SpawnWave();
    }

    void Update(const float &deltaTime) override
    {
        auto &input = InputManager::Instance();

        // Game over check
        if (game_over)
        {
            std::cout << "\n===== GAME OVER =====" << std::endl;
            std::cout << "Final Score: " << score << std::endl;
            std::cout << "Wave: " << wave << std::endl;
            if (input.IsKeyPressed(SDL_SCANCODE_RETURN))
            {
                ResetGame();
            }
            return;
        }

        // Check for asteroid collisions with bullets
        CheckCollisions();

        // Check if all asteroids are destroyed
        if (GetAsteroidCount() == 0 && !next_wave_spawning)
        {
            next_wave_spawning = true;
            next_wave_timer = 2.0f;
        }

        if (next_wave_spawning)
        {
            next_wave_timer -= deltaTime;
            if (next_wave_timer <= 0.0f)
            {
                wave++;
                SpawnWave();
                next_wave_spawning = false;
            }
        }

        // Remove off-screen bullets
        RemoveOffScreenBullets();
    }

    int GetScore() const { return score; }
    int GetWave() const { return wave; }
    int GetLives() const { return lives; }

private:
    Scene *scene;
    std::mt19937 generator;
    int score = 0;
    int wave = 1;
    int lives = 3;
    bool game_over = false;
    bool next_wave_spawning = false;
    float next_wave_timer = 0.0f;

    void SpawnWave()
    {
        int asteroid_count = 2 + wave;
        for (int i = 0; i < asteroid_count; ++i)
        {
            SpawnAsteroid(Asteroid::LARGE);
        }
        std::cout << "Wave " << wave << " started! " << asteroid_count << " asteroids spawned." << std::endl;
    }

    void SpawnAsteroid(Asteroid::Size size)
    {
        auto asteroid = scene->Create("Asteroid#");
        
        // Random spawn position around edges
        std::uniform_real_distribution<> edge_dis(0.0f, 1.0f);
        float edge = edge_dis(generator);
        float x, y;

        if (edge < 0.25f) // Top
        {
            std::uniform_real_distribution<> x_dis(0.0f, 956.0f);
            x = x_dis(generator);
            y = -40.0f;
        }
        else if (edge < 0.5f) // Bottom
        {
            std::uniform_real_distribution<> x_dis(0.0f, 956.0f);
            x = x_dis(generator);
            y = 580.0f;
        }
        else if (edge < 0.75f) // Left
        {
            std::uniform_real_distribution<> y_dis(0.0f, 540.0f);
            y = y_dis(generator);
            x = -40.0f;
        }
        else // Right
        {
            std::uniform_real_distribution<> y_dis(0.0f, 540.0f);
            y = y_dis(generator);
            x = 996.0f;
        }

        if (auto t = asteroid->GetComponent<Transform>())
        {
            t->position = {x, y, 0.0001f};
        }

        // Random velocity
        std::uniform_real_distribution<> vel_dis(-150.0f, 150.0f);
        auto rb = asteroid->AddComponent<Rigidbody2D>();
        rb->mass = 1.0f;
        rb->velocity = {vel_dis(generator), vel_dis(generator)};

        auto ast = asteroid->AddComponent<Asteroid>(scene, size);
    }

    int GetAsteroidCount()
    {
        int count = 0;
        const auto &entities = scene->CreateEntity(nullptr, nullptr)->GetWorld()->GetEntities();
        // This is a simplified check - in real implementation you'd track asteroids differently
        return count;
    }

    void CheckCollisions()
    {
        // Note: Collisions are handled by physics system
        // This is for game logic when asteroids hit bullets
        // You'd need to add collision callbacks to properly implement this
    }

    void RemoveOffScreenBullets()
    {
        // Similar to CheckCollisions, bullets are managed by the bullet component
    }

    void ResetGame()
    {
        score = 0;
        wave = 1;
        lives = 3;
        game_over = false;
        next_wave_spawning = false;
        SpawnWave();
    }
};
