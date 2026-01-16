#shader vertex
#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
    TexCoord = aTexCoord;
    gl_Position = vec4(aPos, 0.0, 1.0);
}

#shader fragment
#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D screenTexture;

void main()
{
    // Simply output the texture color
    FragColor = texture(screenTexture, TexCoord);

    // Optional: gamma correction
    // float gamma = 2.2;
    // FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}
