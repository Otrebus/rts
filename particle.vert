#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;
layout (location = 2) in float aSize;
layout (location = 3) in vec4 aColor;

out vec2 TexCoord;
out vec3 VertexPosition;
out float Size;
out vec4 Color;

void main()
{
    TexCoord = aTex;
    VertexPosition = aPos;
    Size = aSize;
    Color = aColor;
}