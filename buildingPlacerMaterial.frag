#version 450 core

uniform vec3 Kd;

in vec2 texCoord;

out vec4 FragColor;

void main()
{
    vec2 t = texCoord;
    FragColor = vec4(Kd, 1.0);
}
