#include <game.h>

float fixedTime = 0.0;

void Update(float dt)
{
    if (!gameState->initialized)
    {
        InitGame();
        gameState->initialized = true;
    }

    fixedTime += dt;

    while (fixedTime >= FIXED_DELTATIME)
    {
        simulate();
        step();
        fixedTime -= FIXED_DELTATIME;
    }

    render();
}

void InitGame()
{
    Player& player = gameState->player;
    player.pos = vec2(0.0f);
    player.size = vec2(32.0f);
}

void simulate() {}
void step() {}

void render()
{
    Player& player = gameState->player;

    DrawSprite(SPRITE_CELESTE, player.pos, player.size);
    DrawUIText("Hello World", vec2(100, 200), 1.0f);
}
