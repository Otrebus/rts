#version 450 core

uniform vec3 Kd;
out vec4 FragColor;

void main()
{
    FragColor = vec4(Kd, 1.0f);
}
