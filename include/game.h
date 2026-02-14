#pragma once
#include <assets.h>
#include <interface.h>
#include <platform/win32.h>

constexpr float FIXED_DELTATIME = 1.0f / 60.0f;

constexpr int WORLD_WIDTH = 950;
constexpr int WORLD_HEIGHT = 540;
constexpr int TILESIZE = 32;
constexpr ivec2 WORLD_GRID = {WORLD_WIDTH / TILESIZE, WORLD_HEIGHT / TILESIZE};

struct Player
{
    vec2 pos;
    vec2 size;
};

struct Tile
{
    int neighbourMask;
    bool isVisible;
};

struct GameState
{
    Player player;

    Array<IVec2> tileCoords;
    Tile worldGrid[WORLD_GRID.x][WORLD_GRID.y];
    bool initialized{false};

    GameState()
    {
        for (int y = 0; y < WORLD_GRID.y; y++)
            for (int x = 0; x < WORLD_GRID.x; x++)
                worldGrid[x][y] = Tile{};
    }
};

extern GameState *gameState;

void InitGame();
void simulate();
void step();
void render();

void Update(float dt);