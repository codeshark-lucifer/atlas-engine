#pragma once
#include <utils.h>

struct Transform
{
    ivec2 ioffset;
    ivec2 isize;

    vec2 pos;
    vec2 size;
};

struct RenderData
{
    Array<Transform> transforms;
    Array<Transform> uiTransforms;
};

extern Input *input;
extern RenderData *renderData;