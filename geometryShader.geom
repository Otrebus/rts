#version 450 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec2 TexCoord[3]; // Input texture coordinates from the vertex shader
in vec3 VertexPosition[3]; // Input vertex positions from the vertex shader
in vec3 normal[3]; // Input normals from the vertex shader
in vec3 toCam[3]; // Input toCam vectors from the vertex shader

out vec2 texCoord;
out vec3 N_g;
out vec3 N_s;
out vec3 tocam;

void main() {
    for (int i = 0; i < 3; ++i) {
        gl_Position = gl_in[i].gl_Position;

        // Calculate the geometric normal (cross product of two edges)
        vec3 edge1 = VertexPosition[1] - VertexPosition[0];
        vec3 edge2 = VertexPosition[2] - VertexPosition[0];
        vec3 geometricNormal = normalize(cross(edge1, edge2));
        N_s = normal[i];
        texCoord = TexCoord[i];
        N_g = geometricNormal; // Pass the geometric normal
        tocam = toCam[i]; // Pass the toCam vector

        EmitVertex();
    }
    EndPrimitive();
}