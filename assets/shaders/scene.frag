#version 430 core
layout (location = 0) in vec2 textureCoordsIn;

layout (location = 0) out vec4 fragColor;

layout (binding = 0) uniform sampler2D textureAtlas;

void main() {
    vec4 textureColor = texelFetch(textureAtlas, ivec2(textureCoordsIn), 0);

    if(textureColor.r == 0.0)
      discard;

    fragColor = textureColor;
}