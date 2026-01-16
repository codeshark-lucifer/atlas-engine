#version 460 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

in VS_OUT {
    vec4 FragPos;
} gs_in[];

uniform mat4 shadowMatrices[6];

out vec4 FragPos; // Pass FragPos to fragment shader

void main()
{
    for (int face = 0; face < 6; ++face)
    {
        gl_Layer = face; // Selects the face of the cubemap to render to
        for (int i = 0; i < 3; ++i)
        {
            FragPos = gs_in[i].FragPos;
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}
