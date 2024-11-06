#version 450 core

struct PointLight {    
    vec3 position;
    vec3 color;
};  

uniform PointLight pointLights[100];
uniform int nLights;

uniform vec3 Kd;

in vec3 N_s;
in vec3 N_g; // Updated to use the geometric normal
in vec3 tocam; // Updated to use the toCam vector

out vec4 FragColor;

void main()
{
    vec3 ambient = 0.2*Kd;
    vec3 color = ambient;

    color = clamp(color, 0.0, 1.0);
    FragColor = vec4(color, 0.5);
}
