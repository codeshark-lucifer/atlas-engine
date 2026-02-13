#pragma once
#include "vector.h"

struct Rect
{
    Vec2 pos;
    Vec2 size;
};

struct IRect
{
    Vec2 pos;
    IVec2 size;
};

inline bool point_in_rect(const Vec2 &point, const Rect &rect)
{
    return (point.x >= rect.pos.x &&
            point.x <= rect.pos.x + rect.size.x &&
            point.y >= rect.pos.y &&
            point.y <= rect.pos.y + rect.size.y);
}

inline bool point_in_rect(const Vec2 &point, const IRect &rect)
{
    return (point.x >= rect.pos.x &&
            point.x <= rect.pos.x + float(rect.size.x) &&
            point.y >= rect.pos.y &&
            point.y <= rect.pos.y + float(rect.size.y));
}

inline bool rect_collision(const IRect &a, const IRect &b)
{
    return a.pos.x < b.pos.x + float(b.size.x) &&
           a.pos.x + float(a.size.x) > b.pos.x &&
           a.pos.y < b.pos.y + float(b.size.y) &&
           a.pos.y + float(a.size.y) > b.pos.y;
}

inline bool rect_collision(const Rect &a, const Rect &b)
{
    return a.pos.x < b.pos.x + b.size.x &&
           a.pos.x + a.size.x > b.pos.x &&
           a.pos.y < b.pos.y + b.size.y &&
           a.pos.y + a.size.y > b.pos.y;
}
