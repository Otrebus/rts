#version 450 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec2 TexCoord[3];
in vec3 VertexPosition[3];
in vec3 normal[3];
in vec3 toCam[3];

out vec2 texCoord;
out vec3 N_g;
out vec3 N_s;
out vec3 tocam;

void main() {
    for (int i = 0; i < 3; ++i) {
        gl_Position = gl_in[i].gl_Position;

        vec3 edge1 = VertexPosition[1] - VertexPosition[0];
        vec3 edge2 = VertexPosition[2] - VertexPosition[0];
        vec3 geometricNormal = normalize(cross(edge1, edge2));
        N_s = normal[i];
        texCoord = TexCoord[i];
        N_g = geometricNormal;
        tocam = toCam[i];

        EmitVertex();
    }
    EndPrimitive();
}