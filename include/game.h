#pragma once
#include <assets.h>
#include <interface.h>
#include <platform/win32.h>
#include <systems/particle.h>

constexpr float FIXED_DELTATIME = 1.0f / 60.0f;

constexpr int WORLD_WIDTH = 950;
constexpr int WORLD_HEIGHT = 540;
constexpr int TILESIZE = 32;
constexpr int GRID_X = (WORLD_WIDTH + TILESIZE - 1) / TILESIZE;
constexpr int GRID_Y = (WORLD_HEIGHT + TILESIZE - 1) / TILESIZE;
constexpr ivec2 WORLD_GRID = {GRID_X, GRID_Y};

enum PlayerAnimState
{
    PLAYER_ANIM_IDLE,
    PLAYER_ANIM_RUN,
    PLAYER_ANIM_JUMP,

    PLAYER_ANIM_COUNT
};

struct Player
{
    vec2 pos;
    vec2 size;
    vec2 vel;        // Added velocity
    bool isGrounded; // To prevent infinite jumping
    int renderOptions;
    float animtime;
    PlayerAnimState animationState;
    SpriteID animationSprites[PLAYER_ANIM_COUNT];
};

struct Tile
{
    int neighbourMask;
    bool isVisible;
};

struct Coin
{
    vec2 pos;
    vec2 size;
    bool collected;
};

struct GameState
{
    Player player;

    Array<IVec2> tileCoords;
    Tile worldGrid[WORLD_GRID.x][WORLD_GRID.y];
    bool initialized{false};

    Array<Coin> coins;
    int coinCount = 0;

    GameState()
    {
        for (int y = 0; y < WORLD_GRID.y; y++)
            for (int x = 0; x < WORLD_GRID.x; x++)
                worldGrid[x][y] = Tile{0, false};

        tileCoords.resize(21);
    }
};

extern GameState *gameState;
static ParticleSystem particle;

void InitGame();
void simulate();
void step();
void render();

void Update(float dt);