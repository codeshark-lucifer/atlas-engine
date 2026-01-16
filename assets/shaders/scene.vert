#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out VS_OUT {
    vec2 TexCoord;
    vec3 FragPos;
    vec3 Normal;
    vec4 FragPosLightSpace[16];
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 lightViewProjection[16];
uniform int lightCount;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    
    vs_out.FragPos = worldPos.xyz;
    vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;
    vs_out.TexCoord = aTexCoord;

    for(int i = 0; i < lightCount; i++) {
        vs_out.FragPosLightSpace[i] = lightViewProjection[i] * worldPos;
    }

    gl_Position = projection * view * worldPos;
}
