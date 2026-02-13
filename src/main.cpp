#include <platform/win32.h>
#include <render_types.h>
#include <glad/glad.h>
#include <shader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>
#include <string>
#include <vector>

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

struct Glyph
{
    ivec2 size;    // glyph bitmap size
    ivec2 bearing; // offset from baseline
    u32 advance;   // advance.x from FreeType
    ivec2 offset;  // atlas position (ioffset)
};

struct Font
{
    std::map<char, Glyph> glyphs;
    u32 texture;
    int atlasWidth;
    int atlasHeight;
};

struct GLContext
{
    Shader *shader;
    u32 vao;
};

static GLContext gl;
static Font font;
bool LoadFont(const char *path, int pixelSize)
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
        return false;

    FT_Face face;
    if (FT_New_Face(ft, path, 0, &face))
        return false;

    FT_Set_Pixel_Sizes(face, 0, pixelSize);

    const int firstChar = 32;
    const int lastChar = 126;

    int atlasWidth = 0;
    int atlasHeight = 0;

    // Calculate atlas size (simple horizontal packing)
    for (int c = firstChar; c <= lastChar; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            continue;

        atlasWidth += face->glyph->bitmap.width;
        atlasHeight = std::max(atlasHeight, (int)face->glyph->bitmap.rows);
    }

    font.atlasWidth = atlasWidth;
    font.atlasHeight = atlasHeight;

    std::vector<unsigned char> atlas(atlasWidth * atlasHeight, 0);

    int x = 0;
    const int padding = 2;
    for (int c = firstChar; c <= lastChar; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            continue;

        FT_Bitmap &bmp = face->glyph->bitmap;

        for (int row = 0; row < bmp.rows; row++)
        {
            for (int col = 0; col < bmp.width; col++)
            {
                int atlasIndex = (row * atlasWidth) + (x + col);
                atlas[atlasIndex] = bmp.buffer[row * bmp.pitch + col];
            }
        }

        Glyph glyph;
        glyph.size = ivec2(bmp.width, bmp.rows);
        glyph.bearing = ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
        glyph.advance = face->glyph->advance.x;
        glyph.offset = ivec2(x, 0);

        font.glyphs.insert({(char)c, glyph});

        x += bmp.width;
    }

    // Upload atlas as GL texture
    glGenTextures(1, &font.texture);
    glBindTexture(GL_TEXTURE_2D, font.texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
                 atlasWidth, atlasHeight,
                 0, GL_RED, GL_UNSIGNED_BYTE,
                 atlas.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    return true;
}

void DrawUIText(std::string text, vec2 pos, float scale)
{
    float x = pos.x;
    float y = pos.y;
    for (char c : text)
    {
        if (font.glyphs.find(c) == font.glyphs.end())
            continue;

        Glyph &g = font.glyphs[c];

        float xpos = x + g.bearing.x * scale;
        float ypos = y - g.bearing.y * scale; // Adjust this based on your screen space

        vec2 size = vec2(g.size.x, g.size.y) * scale;

        renderData->uiTransforms.push_back({g.offset, g.size, vec2(xpos, ypos), size});
        x += (g.advance >> 6) * scale; // FreeType advance is 1/64 pixels
    }
}

void Update(float dt);
int w, h, ch;
int main()
{
    InitPlatform();
    gameState = BumpAlloc<GameState>(&persistentStorage);

    CreateWindowPlatform(956, 540, "atlas - engine");
    SetColorTitleBar({0.1f, 0.1f, 0.1f, 1.0f}, COLOR_WHITE);
    gl.shader = BumpAlloc<Shader>(&persistentStorage, "assets/shaders/scene.vert", "assets/shaders/scene.frag");

    u32 transSSBO;
    u32 m_texture;
    {
        glGenBuffers(1, &transSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, transSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Transform) * 0, 0, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, transSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    {
        unsigned char *image = stbi_load("assets/textures/sample.png", &w, &h, &ch, STBI_rgb_alpha);

        if (!image)
            return EXIT_FAILURE;

        glGenTextures(1, &m_texture);
        glBindTexture(GL_TEXTURE_2D, m_texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(image);
    }

    LoadFont("assets/fonts/arial.ttf", 24);

    gl.shader->Use();
    gl.shader->SetUniform("textureAtlas", 0);

    u32 vao;
    glGenVertexArrays(1, &vao);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    while (!ShouldClose())
    {
        Event event;
        PollEvent(&event);
        {
            mat4 projection = Mat4::Ortho(0, input->screenSize.x, input->screenSize.y, 0, -1, 1);
            gl.shader->Use();
            gl.shader->SetUniform("projection", projection);
            Update(deltaTime);
        }
        glViewport(0, 0, input->screenSize.x, input->screenSize.y);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(gl.vao);
        // ---------- SPRITES ----------
        if (renderData->transforms.size() > 0)
        {
            gl.shader->Use();
            gl.shader->SetUniform("atlasSize", vec2(w, h));
            gl.shader->SetUniform("isFont", false); // ✅ IMPORTANT

            glBindVertexArray(vao);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, transSSBO);
            glBufferData(GL_SHADER_STORAGE_BUFFER,
                         sizeof(Transform) * renderData->transforms.size(),
                         renderData->transforms.data(),
                         GL_DYNAMIC_DRAW);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_texture);

            glDrawArraysInstanced(GL_TRIANGLES, 0, 6,
                                  renderData->transforms.size());
        }

        // ---------- UI TEXT ----------
        if (renderData->uiTransforms.size() > 0)
        {
            gl.shader->Use();
            gl.shader->SetUniform("atlasSize",
                                  vec2(font.atlasWidth, font.atlasHeight));
            gl.shader->SetUniform("isFont", true); // ✅ IMPORTANT

            glBindVertexArray(vao);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, transSSBO);
            glBufferData(GL_SHADER_STORAGE_BUFFER,
                         sizeof(Transform) * renderData->uiTransforms.size(),
                         renderData->uiTransforms.data(),
                         GL_DYNAMIC_DRAW);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, font.texture);

            glDrawArraysInstanced(GL_TRIANGLES, 0, 6,
                                  renderData->uiTransforms.size());
        }

        glBindVertexArray(0);

        renderData->transforms.clear();
        renderData->uiTransforms.clear();
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
    DrawUIText("Hello World", vec2(100, 200), 1.0f);
}