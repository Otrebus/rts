#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in float aSize;
layout (location = 2) in vec3 aColor;

out vec3 VertexPosition;
out float Size;
out vec4 Color;

void main()
{
    VertexPosition = aPos;
    Size = aSize;
    Color = vec4(aColor, 1.0);
}