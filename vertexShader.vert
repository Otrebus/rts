#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;

out vec2 TexCoord;
out vec3 VertexPosition; // Pass vertex positions to the geometry shader
out vec3 normal;
out vec3 toCam;

uniform mat4 transform;
uniform vec3 camPos;

void main()
{
    gl_Position = vec4(aPos, 1.0);  
    TexCoord = aTex;
    VertexPosition = aPos; // Pass the vertex position
    normal = aNorm;
    toCam = camPos - aPos;
    toCam = normalize(toCam);
    gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}