#include <platform/win32.h>
#include <render_types.h>
#include <glad/glad.h>
#include <shader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <ft2build.h>
#include FT_FREETYPE_H

enum SpriteID
{
    SPRITE_WHITE,
    SPRITE_DICE,
    SPRITE_CELESTE
};

struct Sprite
{
    ivec2 offset;
    ivec2 size;
};

Sprite getSprite(SpriteID spriteID)
{
    switch (spriteID)
    {
    case SPRITE_WHITE:
        return {{0, 0}, {1, 1}};
    case SPRITE_DICE:
        return {{16, 0}, {16, 16}};
    case SPRITE_CELESTE:
        return {{112, 0}, {17, 20}};
    default:
        return getSprite(SPRITE_WHITE);
    }
}

void DrawSprite(ivec2 ioffset, ivec2 isize, vec2 pos, vec2 size)
{
    Transform trans = {};
    trans.ioffset = ioffset;
    trans.isize = isize;

    trans.pos = pos;
    trans.size = size;

    renderData->transforms.push_back(trans);
}

void DrawSprite(SpriteID spriteID, vec2 pos, vec2 size)
{
    Sprite sprite = getSprite(spriteID);
    DrawSprite(sprite.offset, sprite.size, pos, size);
}

void DrawSprite(Transform trans)
{
    DrawSprite(trans.ioffset, trans.isize, trans.pos, trans.size);
}

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

GameState *gameState = nullptr;

float FIXED_DELTATIME = 1.0f / 60.0f;
float fixedTime = 0.0f;

void Update(float dt);
int main()
{
    InitPlatform();
    gameState = BumpAlloc<GameState>(&persistentStorage);

    CreateWindowPlatform(956, 540, "atlas - engine");
    SetColorTitleBar({0.1f, 0.1f, 0.1f, 1.0f}, COLOR_WHITE);
    Shader shader("assets/shaders/scene.vert",
                  "assets/shaders/scene.frag");

    u32 transSSBO;
    glGenBuffers(1, &transSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, transSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Transform) * 0, 0, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, transSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    u32 vao;
    glGenVertexArrays(1, &vao);

    int w, h, ch;
    unsigned char *image = stbi_load("assets/textures/sample.png", &w, &h, &ch, STBI_rgb_alpha);

    if (!image)
        return EXIT_FAILURE;

    u32 m_texture;
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(image);

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        LOG_ASSERT(false, "Could not init FreeType Library");
        return -1;
    }

    mat4 projection = Mat4::Ortho(0, input->screenSize.x, input->screenSize.y, 0, -1, 1);
    shader.Use();
    shader.SetUniform("textureAtlas", 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    while (!ShouldClose())
    {
        Event event;
        PollEvent(&event);
        {
            Update(deltaTime);
        }
        glViewport(0, 0, input->screenSize.x, input->screenSize.y);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, transSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Transform) * renderData->transforms.size(), renderData->transforms.data(), GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, transSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        shader.Use();
        shader.SetUniform("projection", projection);
        glBindVertexArray(vao);
        {
            glBindTexture(GL_TEXTURE_2D, m_texture);
            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, renderData->transforms.size());
        }
        glBindVertexArray(0);
        renderData->transforms.clear();
        SwapBuffersWindow();
    }
    return 0;
}

void InitGame();

void simulate();
void step();
void render();

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
        simulate(); // physics / AI / game logic
        step();     // apply changes to game state
        fixedTime -= FIXED_DELTATIME;
    }

    render(); // render interpolated state
}

void InitGame()
{
    Player &player = gameState->player;
    player.pos = vec2(0.0f);
    player.size = vec2(32.0f);
}

void simulate()
{
}

void step()
{
}

void render()
{
    Player &player = gameState->player;
    DrawSprite(SPRITE_CELESTE, player.pos, player.size);
}