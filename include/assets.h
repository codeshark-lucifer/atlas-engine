#pragma once
#include <utils.h>

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

inline Sprite getSprite(SpriteID spriteID)
{
    switch (spriteID)
    {
    case SPRITE_WHITE:   return {{0, 0}, {1, 1}};
    case SPRITE_DICE:    return {{16, 0}, {16, 16}};
    case SPRITE_CELESTE: return {{112, 0}, {17, 20}};
    default:             return getSprite(SPRITE_WHITE);
    }
}