#pragma once
#include <assets.h>
#include <interface.h>

constexpr float FIXED_DELTATIME = 1.0f / 60.0f;

struct Player
{
    vec2 pos;
    vec2 size;
};

struct GameState
{
    Player player;
    bool initialized{false};
};

extern GameState* gameState;

void InitGame();
void simulate();
void step();
void render();

void Update(float dt);