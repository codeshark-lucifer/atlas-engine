#version 430 core

layout (location = 0) in vec2 textureCoordsIn;
layout (location = 0) out vec4 fragColor;
layout (binding = 0) uniform sampler2D textureAtlas;

uniform bool isFont;

void main()
{
    vec4 texColor = texture(textureAtlas, textureCoordsIn);

    // Detect RED-only texture (font)
    if(isFont)
    {
        float alpha = texColor.r;
        if(alpha < 0.01)
            discard;
        fragColor = vec4(1.0, 1.0, 1.0, alpha);
    }
    else
    {
        if(texColor.a < 0.01)
            discard;
        fragColor = texColor;
    }

}
