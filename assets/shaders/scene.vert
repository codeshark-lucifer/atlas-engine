#version 430 core

struct Transform {
    ivec2 ioffset;
    ivec2 isize;

    vec2 pos;
    vec2 size;
};

layout (location = 0) out vec2 textureCoordsOut;
layout (std430, binding = 0) buffer TransformSBO
{
    Transform transforms[];
};

uniform mat4 projection;
uniform vec2 atlasSize;

void main()
{
    Transform transform = transforms[gl_InstanceID];
    vec2 vertices[6] =
    {
        transform.pos,                                        // Top Left
        vec2(transform.pos + vec2(0.0, transform.size.y)),    // Bottom Left
        vec2(transform.pos + vec2(transform.size.x, 0.0)),    // Top Right
        vec2(transform.pos + vec2(transform.size.x, 0.0)),    // Top Right
        vec2(transform.pos + vec2(0.0, transform.size.y)),    // Bottom Left
        transform.pos + transform.size                        // Bottom Right
    };
    
    int left = transform.ioffset.x;
    int top = transform.ioffset.y;
    int right = transform.ioffset.x + transform.isize.x;
    int bottom = transform.ioffset.y + transform.isize.y;

    vec2 textureCoords[6] = 
    {
        vec2(left,  top)    / atlasSize,
        vec2(left,  bottom) / atlasSize,
        vec2(right, top)    / atlasSize,
        vec2(right, top)    / atlasSize,
        vec2(left,  bottom) / atlasSize,
        vec2(right, bottom) / atlasSize,
    };

    textureCoordsOut = textureCoords[gl_VertexID];
    vec2 vertex = vertices[gl_VertexID];
    gl_Position = projection * vec4(vertex, 0.0, 1.0);
}
