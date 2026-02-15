#include <game.h>

float fixedTime = 0.0f;
bool ishidden = false;

Tile *getTile(int x, int y)
{
    Tile *tile = nullptr;
    if (x >= 0 && x < WORLD_GRID.x &&
        y >= 0 && y < WORLD_GRID.y)
    {
        tile = &gameState->worldGrid[x][y];
    }
    return tile;
}

Tile *getTile(ivec2 worldPos)
{
    return getTile(worldPos.x, worldPos.y);
}
void UpdateTile()
{
    // Screen Space Offsets: Y+ is Down, Y- is Up
    int offsets[24] = {
        0, -1, // 0: Up (North) - changed from 0, 1
        -1, 0, // 1: Left (West)
        1, 0,  // 2: Right (East)
        0, 1,  // 3: Down (South) - changed from 0, -1

        -1, -1, // 4: Top-Left
        1, -1,  // 5: Top-Right
        -1, 1,  // 6: Bottom-Left
        1, 1,   // 7: Bottom-Right

        0, -2, // 8: Extended Up
        -2, 0, // 9: Extended Left
        2, 0,  // 10: Extended Right
        0, 2   // 11: Extended Down
    };

    for (int y = 0; y < WORLD_GRID.y; y++)
        for (int x = 0; x < WORLD_GRID.x; x++)
        {
            Tile *tile = getTile(x, y);
            if (!tile || !tile->isVisible)
                continue;

            tile->neighbourMask = 0;
            int neighborCount = 0, extendedCount = 0, emptySlot = 0;

            for (int n = 0; n < 12; n++)
            {
                Tile *neighbor = getTile(x + offsets[n * 2], y + offsets[n * 2 + 1]);
                if (!neighbor || neighbor->isVisible)
                {
                    tile->neighbourMask |= BIT(n);
                    if (n < 8)
                        neighborCount++;
                    else
                        extendedCount++;
                }
                else if (n < 8)
                    emptySlot = n;
            }

            if (neighborCount == 7 && emptySlot >= 4)
                tile->neighbourMask = 16 + (emptySlot - 4);
            else if (neighborCount == 8 && extendedCount == 4)
                tile->neighbourMask = 20;
            else
                tile->neighbourMask &= 0b1111;
        }
}

void InitGame()
{
    // According to your image, the blue tileset is roughly
    // in a 4x4 or 5x5 grid area starting at (48, 0)
    int startX = 48;
    int startY = 0;
    int spriteSize = 8;
    int atlasColumns = 4; // How many tiles wide the blue cluster is

    for (int i = 0; i < 21; i++)
    {
        int row = i / atlasColumns;
        int col = i % atlasColumns;

        gameState->tileCoords[i] = ivec2(
            startX + (col * spriteSize),
            startY + (row * spriteSize));
    }

    // --- Add Default Platform ---
    int centerX = WORLD_GRID.x / 2;
    int centerY = WORLD_GRID.y / 2;

    // Create a 3-tile wide platform
    for (int x = centerX - 6; x <= centerX + 6; x++)
    {
        Tile *tile = getTile(x, centerY);
        if (tile)
        {
            tile->isVisible = true;
        }
    }

    // Refresh neighbor masks so the platform looks connected
    UpdateTile();

    Player &player = gameState->player;
    player.animationSprites[PLAYER_ANIM_IDLE] = SPRITE_CELESTE;
    player.animationSprites[PLAYER_ANIM_RUN] = SPRITE_CELESTE_RUN;
    player.animationSprites[PLAYER_ANIM_JUMP] = SPRITE_CELESTE_JUMP;

    player.pos = vec2(centerX * TILESIZE, (centerY - 2) * TILESIZE);
    player.size = vec2(34.0f, 40.0f); // Celeste is 17x20, so 2x scale works well
    player.vel = vec2(0, 0);
    player.animationState = PLAYER_ANIM_IDLE;
}

void simulate()
{
    Player &player = gameState->player;
    PlayerAnimState lastState = player.animationState;

    // Default state
    player.animationState = PLAYER_ANIM_IDLE;

    if (input->IsKeyPressed(KEY_ESCAPE))
    {
        ishidden = false;
        SetMouseMode(MOUSE_VISIBLE);
    }

    if (input->IsMousePressed(MOUSE_LEFT) && !ishidden)
    {
        ishidden = true;
        SetMouseMode(MOUSE_HIDDEN);
    }

    if (input->IsMousePressed(MOUSE_LEFT))
    {
        int gridX = (int)(input->mousePosScreen.x / TILESIZE);
        int gridY = (int)(input->mousePosScreen.y / TILESIZE);

        Tile *tile = getTile(gridX, gridY);
        if (tile)
        {
            tile->isVisible = true;
            UpdateTile();
        }
    }

    if (input->IsMousePressed(MOUSE_RIGHT))
    {
        int gridX = (int)(input->mousePosScreen.x / TILESIZE);
        int gridY = (int)(input->mousePosScreen.y / TILESIZE);

        Tile *tile = getTile(gridX, gridY);
        if (tile)
        {
            tile->isVisible = false;
            UpdateTile();
        }
    }

    float moveSpeed = 200.0f;
    float jumpForce = -400.0f;

    player.vel.x = 0;
    if (input->IsKeyPressed(KEY_SPACE) && player.isGrounded)
    {
        player.vel.y = jumpForce;
        player.isGrounded = false;
    }

    if (input->IsKeyHeld(KEY_A))
    {
        player.vel.x = -moveSpeed;
        player.renderOptions |= RENDERING_OPTION_FLIP_X;
        player.animationState = PLAYER_ANIM_RUN;
    }
    else if (input->IsKeyHeld(KEY_D))
    {
        player.vel.x = moveSpeed;
        player.renderOptions &= ~RENDERING_OPTION_FLIP_X;
        player.animationState = PLAYER_ANIM_RUN;
    }

    if (!player.isGrounded)
    {
        player.animationState = PLAYER_ANIM_JUMP;
    }
    if (player.animationState != lastState)
    {
        player.animtime = 0.0f;
    }
    // 1. Get the current sprite to know the frame count
    SpriteID currentID = player.animationSprites[player.animationState];
    Sprite sprite = getSprite(currentID);

    // 2. Increment by delta time, not a whole integer
    player.animtime += FIXED_DELTATIME;

    // 3. Reset logic (Optional here, as animate() also handles wrapping)
    if (player.animtime >= 0.6f) // 0.6f is your duration
        player.animtime = 0.0f;
}

void step()
{
    Player &player = gameState->player;
    float gravity = 1200.0f;
    player.vel.y += gravity * FIXED_DELTATIME;

    // Calculate the range of tiles the player is currently overlapping
    int minX = (int)(player.pos.x / TILESIZE) - 1;
    int maxX = (int)((player.pos.x + player.size.x) / TILESIZE) + 1;
    int minY = (int)(player.pos.y / TILESIZE) - 1;
    int maxY = (int)((player.pos.y + player.size.y) / TILESIZE) + 1;

    // 1. Move X
    player.pos.x += player.vel.x * FIXED_DELTATIME;
    for (int y = minY; y <= maxY; y++)
    {
        for (int x = minX; x <= maxX; x++)
        {
            Tile *tile = getTile(x, y);
            if (tile && tile->isVisible)
            {
                Rect tileRect = {vec2(x * TILESIZE, y * TILESIZE), vec2(TILESIZE)};
                if (CheckCollision({player.pos, player.size}, tileRect))
                {
                    if (player.vel.x > 0)
                        player.pos.x = tileRect.pos.x - player.size.x;
                    else if (player.vel.x < 0)
                        player.pos.x = tileRect.pos.x + tileRect.size.x;
                    player.vel.x = 0;
                }
            }
        }
    }

    // 2. Move Y
    player.pos.y += player.vel.y * FIXED_DELTATIME;
    player.isGrounded = false;
    for (int y = minY; y <= maxY; y++)
    {
        for (int x = minX; x <= maxX; x++)
        {
            Tile *tile = getTile(x, y);
            if (tile && tile->isVisible)
            {
                Rect tileRect = {vec2(x * TILESIZE, y * TILESIZE), vec2(TILESIZE)};
                if (CheckCollision({player.pos, player.size}, tileRect))
                {
                    if (player.vel.y > 0)
                    {
                        player.pos.y = tileRect.pos.y - player.size.y;
                        player.isGrounded = true;
                        player.vel.y = 0;
                    }
                    else if (player.vel.y < 0)
                    {
                        player.pos.y = tileRect.pos.y + tileRect.size.y;
                        player.vel.y = 0;
                    }
                }
            }
        }
    }
}
void DrawGrid();
void render()
{
    DrawGrid();

    {
        Player &player = gameState->player;
        SpriteID currentID = player.animationSprites[player.animationState];
        Sprite sprite = getSprite(currentID);

        // Use a consistent duration (e.g., 0.5 seconds for a full loop)
        float animationLoopDuration = 0.5f;
        int anim_x = animate(&player.animtime, sprite.frameCount, animationLoopDuration);

        DrawSprite(sprite, player.pos, player.size, {anim_x, player.renderOptions, 0.0f});
    }
    vec2 mouse = input->mousePosScreen;
    DrawSprite(SPRITE_REDBALL, mouse - vec2(4.0f), vec2(8.0f));

    DrawUIText("Game", vec2(10.0f), 24.0f);
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

void DrawGrid()
{
    for (int y = 0; y < WORLD_GRID.y; y++)
    {
        for (int x = 0; x < WORLD_GRID.x; x++)
        {
            Tile *tile = getTile(x, y);
            if (tile && tile->isVisible)
            {
                Transform trans = {};
                trans.color = vec4(1.0f);

                // 1. Position in the PNG (Corrected via InitGame)
                trans.ioffset = gameState->tileCoords[tile->neighbourMask];

                // 2. Size in the PNG (Must be 8 because your sprites are 8x8)
                trans.isize = ivec2(8);

                // 3. Position on Screen
                trans.pos = vec2(x * TILESIZE, y * TILESIZE);

                // 4. Size on Screen (Match TILESIZE so they touch each other)
                trans.size = vec2(TILESIZE);

                DrawQuad(trans);
            }
        }
    }
}