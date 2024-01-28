#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;

out vec2 TexCoord;
out vec3 VertexPosition;
out vec3 normal;
out vec3 toCam;

uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;
uniform mat4 projectionMatrix;
uniform vec3 camPos;

void main()
{
    TexCoord = aTex;

    vec4 mPos = modelViewMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    toCam = camPos - vec3(mPos.x, mPos.y, mPos.z);
    toCam = normalize(toCam);

    VertexPosition = vec3(mPos.x, mPos.y, mPos.z);

    gl_Position = projectionMatrix * mPos;

    vec4 nr;
    nr = normalMatrix * vec4(aNorm.x, aNorm.y, aNorm.z, 0);
    normal = vec3(nr.x, nr.y, nr.z);
}