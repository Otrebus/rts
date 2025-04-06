#version 450 core

layout (location = 0) in vec3 aPos;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

out vec3 pos;

void main()
{
    vec4 mPos = modelViewMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    pos = mPos.xyz;
    gl_Position = projectionMatrix * mPos;
}