#version 450 core

layout (lines) in;
layout (line_strip, max_vertices=2) out;

in vec3 pos[2];

out vec3 position;

void main()
{
    for(int i = 0; i < 2; i++)
    {
        gl_Position = gl_in[i].gl_Position;
        position = pos[i];
        EmitVertex();
    }
    EndPrimitive();
}
