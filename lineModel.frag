#version 450 core

out vec4 FragColor;
uniform vec3 Kd;

in vec3 position;

void main()
{
    FragColor = vec4(Kd, 1);
}
