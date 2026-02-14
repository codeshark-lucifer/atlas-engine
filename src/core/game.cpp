#include <game.h>

float fixedTime = 0.0f;

Tile *getTile(int x, int y)
{
    Tile *tile = nullptr;
    if (x >= 0 && x < WORLD_GRID.x && y >= 0 && y < WORLD_GRID.y)
        tile = &gameState->worldGrid[x][y];
    return tile;
}

void UpdateTile()
{
    int offsets[24] = {
        0, -1, // 0: Up (North) - Was 0, 1
        -1, 0, // 1: Left (West)
        1, 0,  // 2: Right (East)
        0, 1,  // 3: Down (South) - Was 0, -1

        -1, -1, // 4: Top-Left (Up-Left)
        1, -1,  // 5: Top-Right (Up-Right)
        -1, 1,  // 6: Bottom-Left (Down-Left)
        1, 1,   // 7: Bottom-Right (Down-Right)

        0, -2, // 8: Extended Up
        -2, 0, // 9: Extended Left
        2, 0,  // 10: Extended Right
        0, 2   // 11: Extended Down
    };

    for (int y = 0; y < WORLD_GRID.y; y++)
    {
        for (int x = 0; x < WORLD_GRID.x; x++)
        {
            Tile *tile = getTile(x, y);
            if (!tile || !tile->isVisible)
                continue;

            int cardinalMask = 0;
            int neighborCount = 0;
            int extendedCount = 0;
            int firstEmptyCorner = -1;

            for (int n = 0; n < 12; n++)
            {
                Tile *neighbor = getTile(x + offsets[n * 2], y + offsets[n * 2 + 1]);

                // We count it as a "neighbor" if it's visible or out of bounds (usually)
                if (neighbor && neighbor->isVisible)
                {
                    if (n < 4)
                        cardinalMask |= (1 << n); // Binary 1, 2, 4, 8
                    if (n < 8)
                        neighborCount++;
                    else
                        extendedCount++;
                }
                else
                {
                    if (n >= 4 && n < 8 && firstEmptyCorner == -1)
                    {
                        firstEmptyCorner = n;
                    }
                }
            }

            // MAPPING LOGIC
            if (neighborCount == 8 && extendedCount == 4)
            {
                // Fully surrounded "Center" tile
                tile->neighbourMask = 20;
            }
            else if (neighborCount == 7 && firstEmptyCorner != -1)
            {
                // Interior Corner tile (16, 17, 18, 19)
                tile->neighbourMask = 16 + (firstEmptyCorner - 4);
            }
            else
            {
                // Standard edge/corner based on Cardinal Mask (0-15)
                // This ensures we stay within the first 16 tiles of your sheet
                tile->neighbourMask = cardinalMask;
            }
        }
    }
}

void InitGame()
{
    Player &player = gameState->player;
    player.pos = vec2(0.0f);
    player.size = vec2(32.0f);

    IVec2 tilesPos = {48, 0};
    for (int y = 0; y < 5; y++)
        for (int x = 0; x < 4; x++)
            gameState->tileCoords.push_back({tilesPos.x + x * 8, tilesPos.y + y * 8});

    gameState->tileCoords.push_back({tilesPos.x, tilesPos.y + 5 * 8});
}

void simulate()
{
    if (input->IsKeyPressed(KEY_ESCAPE))
        SetMouseMode(MOUSE_VISIBLE);

    if (input->IsMousePressed(MOUSE_LEFT))
        SetMouseMode(MOUSE_HIDDEN);

    if (input->IsMousePressed(MOUSE_LEFT))
    {
        vec2 mouse = input->mousePosScreen * (1 / (float)TILESIZE);
        Tile *tile = getTile((int)mouse.x, (int)mouse.y);
        if (tile)
        {
            tile->isVisible = true;
            UpdateTile();
        }
    }
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
            Tile *tile = getTile(x, y);
            if (!tile || !tile->isVisible)
                continue;
            Transform trans = {};
            // trans.ioffset = {32, 0};
            trans.ioffset = gameState->tileCoords[tile->neighbourMask];
            trans.isize = ivec2(8);
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
