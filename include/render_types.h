#pragma once
#include <utils.h>

struct Transform {
    ivec2 ioffset; // 8 bytes
    ivec2 isize;   // 8 bytes
    vec2 pos;      // 8 bytes
    vec2 size;     // 8 bytes
    vec4 color;    // 12 bytes
}; // Total: 44 bytes

struct Camera2D  {
    vec2 pos;
    vec2 dimensions;
    mat4 matrix() {
        mat4 p;
        p = Mat4::Ortho(0, dimensions.x, dimensions.y, 0, -1, 1);
        return p;
    }
};

struct RenderData
{
    Camera2D camera;
    Array<Transform> transforms;
    Array<Transform> uiTransforms;
    void OnResize(int x, int y) {
        camera.dimensions = {(float)x, (float)y};
    }
};

struct Glyph
{
    ivec2 size;    // glyph bitmap size
    ivec2 bearing; // offset from baseline
    u32 advance;   // advance.x from FreeType
    ivec2 offset;  // atlas position (ioffset)
};

extern Input *input;
extern RenderData *renderData;