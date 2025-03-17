#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in float param;

uniform mat4 transform;

out vec3 VertexPosition;
out float par;

void main()
{
    VertexPosition = (transform*vec4(aPos, 1.0)).xyz;
    gl_Position = transform*vec4(aPos, 1.0);
    par = param;
}
