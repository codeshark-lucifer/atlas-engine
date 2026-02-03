#shader vertex
#version 460 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 vUV;

void main()
{
    vUV = aUV;
    gl_Position = projection * view * model * vec4(aPos, 0.0, 1.0);
}

#shader fragment
#version 460 core

in vec2 vUV;
out vec4 FragColor;

uniform sampler2D diffuse_texture1;
uniform vec4 color;

void main()
{
    vec3 albedo = texture(diffuse_texture1, vUV).rgb;
    FragColor = color * vec4(albedo, 1.0);
}
