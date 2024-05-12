#version 450 core

uniform vec3 Kd;

in vec3 N_s;
in vec3 N_g;
in vec3 tocam;

out vec4 FragColor;

void main()
{
    FragColor = vec4(Kd, 0.0);
}