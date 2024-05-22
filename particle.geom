#version 450 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in vec3 VertexPosition[];
in float Size[];
in vec4 Color[];

out vec4 color;

void main() {
    vec3 position = VertexPosition[0]; // Position should be set to NDC coordinates
    float size = 0.2;

    vec2 offsets[4] = vec2[](
        vec2(-size, -size),
        vec2(size, -size),
        vec2(-size, size),
        vec2(size, size)
    );

    for (int i = 0; i < 4; ++i) {
        gl_Position = vec4(0 + offsets[i], 0, 1.0);
        color = Color[0];
        EmitVertex();
    }
    EndPrimitive();
}