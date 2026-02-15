#pragma once
#include <assets.h>
#include <interface.h>
#include <platform/win32.h>

constexpr float FIXED_DELTATIME = 1.0f / 60.0f;

constexpr int WORLD_WIDTH = 950;
constexpr int WORLD_HEIGHT = 540;
constexpr int TILESIZE = 32;
constexpr int GRID_X = (WORLD_WIDTH + TILESIZE - 1) / TILESIZE;
constexpr int GRID_Y = (WORLD_HEIGHT + TILESIZE - 1) / TILESIZE;
constexpr ivec2 WORLD_GRID = {GRID_X, GRID_Y};

struct Player
{
    vec2 pos;
    vec2 size;
    vec2 vel;      // Added velocity
    bool isGrounded; // To prevent infinite jumping
    int renderOptions;
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
                worldGrid[x][y] = Tile{0, false};

        tileCoords.resize(21);
    }
};

extern GameState *gameState;

void InitGame();
void simulate();
void step();
void render();

void Update(float dt);