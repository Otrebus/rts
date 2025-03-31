#version 450 core

out vec4 FragColor;
uniform vec4 Kd;

in vec3 position;

void main()
{
    FragColor = Kd;
}
