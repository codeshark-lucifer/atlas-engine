#pragma once
#include <string>
#include <assets.h>
#include <gl_renderer.hpp>

inline void DrawSprite(SpriteID spriteID, vec2 pos, vec2 size, vec3 color = vec3(1.0f))
{
    Sprite sprite = getSprite(spriteID);
    PushSprite(sprite.offset, sprite.size, pos, size, color, 0);
}

inline void DrawSprite(Sprite sprite, vec2 pos, vec2 size, DrawData data)
{
    // Calculate horizontal frame offset: start_x + (frame_index * frame_width)
    ivec2 animatedOffset = sprite.offset;
    animatedOffset.x += (data.anim_x * sprite.size.x);
    
    // Pass the NEW animatedOffset, not sprite.offset
    PushSprite(animatedOffset, sprite.size, pos, size, vec3(1.0f), data.renderOptions);
}

inline void DrawQuad(Transform trans)
{
    PushSprite(trans.ioffset, trans.isize, trans.pos, trans.size, trans.color.xyz(), trans.renderOptions);
}

inline void DrawUIText(const std::string &text, vec2 pos, float scale, vec3 color = vec3(1.0f))
{
    float x = pos.x;
    float y = pos.y;

    for (char c : text)
    {
        if (font.glyphs.find(c) == font.glyphs.end())
            continue;

        Glyph &g = font.glyphs[c];

        float xpos = x + g.bearing.x * scale;
        float ypos = y - g.bearing.y * scale;

        vec2 size = vec2(g.size.x, g.size.y) * scale;

        Transform trans = {};
        trans.ioffset = g.offset;
        trans.isize = g.size;
        trans.pos = vec2(xpos, ypos);
        trans.size = size;
        trans.color = vec4(color, 1.0f);
        renderData->uiTransforms.push_back(trans);

        x += (g.advance >> 6) * scale;
    }
}