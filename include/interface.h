#pragma once
#include <string>
#include <assets.h>
#include <gl_renderer.hpp>

inline void DrawSprite(SpriteID spriteID, vec2 pos, vec2 size)
{
    Sprite sprite = getSprite(spriteID);
    PushSprite(sprite.offset, sprite.size, pos, size);
}

inline void DrawUIText(const std::string& text, vec2 pos, float scale)
{
    float x = pos.x;
    float y = pos.y;

    for (char c : text)
    {
        if (font.glyphs.find(c) == font.glyphs.end())
            continue;

        Glyph& g = font.glyphs[c];

        float xpos = x + g.bearing.x * scale;
        float ypos = y - g.bearing.y * scale;

        vec2 size = vec2(g.size.x, g.size.y) * scale;

        renderData->uiTransforms.push_back(
            {g.offset, g.size, vec2(xpos, ypos), size});

        x += (g.advance >> 6) * scale;
    }
}