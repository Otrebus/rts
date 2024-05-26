#version 450 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

//in vec2 TexCoord[];
in vec3 VertexPosition[];
in float Size[];
in vec4 Color[];

out vec2 texCoord;
out vec4 color;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 camPos;
uniform vec3 camUp;

void main()
{
    vec3 position = VertexPosition[0];
    float size = Size[0];
    vec4 particleColor = Color[0];

    vec3 forward = normalize(position - camPos);
    vec3 right = cross(camUp, forward);

//    vec3 right = vec3(0.1, 0, 0);
//    vec3 up = vec3(0, 0.1, 0);

    vec3 corners[4];
    corners[0] = right - camUp;
    corners[1] = right + camUp;
    corners[2] = -right - camUp;
    corners[3] = -right + camUp;

    vec2 texCoords[4] = vec2[](vec2(1.0, 0.0), vec2(1.0, 1.0), vec2(0.0, 0.0), vec2(0.0, 1.0));

    for (int i = 0; i < 4; ++i)
    {
        gl_Position = projectionMatrix*vec4(position + corners[i]*size, 1.0);

        //gl_Position = vec4(corners[i], 1.0);
        texCoord = texCoords[i];
        color = particleColor;

        EmitVertex();
    }
    EndPrimitive();
}
