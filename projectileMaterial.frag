#version 450 core

struct PointLight {    
    vec3 position;
    vec3 color;
};  

uniform PointLight pointLights[100];
uniform int nLights;

uniform vec3 Kd;

in vec3 N_s;
in vec3 N_g;
in vec3 tocam;

out vec4 FragColor;

void main()
{
    FragColor = vec4(Kd, 1.0);
}