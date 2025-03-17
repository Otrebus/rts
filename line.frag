#version 450 core

struct PointLight
{    
    vec3 position;
    vec3 color;
};  

uniform PointLight pointLights[100];
uniform int nLights;

out vec4 FragColor;
uniform vec3 Kd;
in float param;

void main()
{
    if(param > 0 && int(param)%2 == 0)
        discard;
    FragColor = vec4(Kd, 1.0f);
}
