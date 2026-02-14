#include <game.h>

float fixedTime = 0.0f;

void InitGame()
{
    Player &player = gameState->player;
    player.pos = vec2(0.0f);
    player.size = vec2(32.0f);
    gameState->gamed = true;
}

void simulate()
{
    if (input->IsKeyPressed(KEY_ESCAPE))
        SetMouseMode(MOUSE_VISIBLE);

    if (input->IsMousePressed(MOUSE_LEFT))
        SetMouseMode(MOUSE_HIDDEN);
}

void step()
{
    // Reserved for physics / gameplay
}

void render()
{
    for (int y = 0; y < WORLD_GRID.y; y++)
    {
        for (int x = 0; x < WORLD_GRID.x; x++)
        {
            Transform trans = {};
            trans.ioffset = {32, 0};
            trans.isize = {16, 16};
            trans.pos = vec2(x * TILESIZE, y * TILESIZE);
            trans.size = vec2(TILESIZE);
            trans.color = vec4(1.0f);

            DrawQuad(trans);
        }
    }

    vec2 mouse = input->mousePosScreen;
    DrawSprite(SPRITE_REDBALL, mouse - vec2(4.0f), vec2(8.0f));
}

// Update Entry
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
