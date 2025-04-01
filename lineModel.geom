#version 450 core

layout (lines) in;
layout (line_strip, max_vertices=2) out;

out vec3 position;

void main()
{
    for(int i = 0; i < 2; i++)
    {
        gl_Position = gl_in[i].gl_Position;
        position = gl_in[i].gl_Position.xyz;
        EmitVertex();
    }
    EndPrimitive();
}
