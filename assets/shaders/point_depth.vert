#version 460 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;

out VS_OUT {
    vec4 FragPos;
} vs_out;

void main()
{
    vs_out.FragPos = model * vec4(aPos, 1.0);
}
