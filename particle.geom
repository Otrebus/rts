#version 450 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in vec2 TexCoord[];
in vec3 VertexPosition[];
in float Size[];
in vec4 Color[];

out vec2 texCoord;
out vec4 color;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    vec3 position = VertexPosition[0];
    float size = Size[0];
    vec4 particleColor = Color[0];

    vec3 cameraRight = vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
    vec3 cameraUp = vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);

    vec3 right = cameraRight * size;
    vec3 up = cameraUp * size;

    vec3 corners[4];
    corners[0] = position - right - up;
    corners[1] = position + right - up;
    corners[2] = position - right + up;
    corners[3] = position + right + up;

    vec2 texCoords[4] = vec2[](vec2(0.0, 0.0), vec2(1.0, 0.0), vec2(0.0, 1.0), vec2(1.0, 1.0));

    for (int i = 0; i < 4; ++i)
    {
        gl_Position = projectionMatrix * vec4(corners[i], 1.0);
        texCoord = texCoords[i];
        color = particleColor;

        EmitVertex();
    }
    EndPrimitive();