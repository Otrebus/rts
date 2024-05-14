#version 330 core

struct PointLight {    
    vec3 position;
    vec3 color;
};  

uniform PointLight pointLights[100];
uniform int nLights;

out vec4 FragColor;
uniform vec3 Kd;

void main() {
    FragColor = vec4(Kd, 1.0f);
}