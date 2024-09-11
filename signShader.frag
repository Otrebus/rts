#version 450 core

struct PointLight {    
    vec3 position;
    vec3 color;
};  

uniform PointLight pointLights[100];
uniform int nLights;

out vec4 FragColor;

in vec2 texCoord;

in vec3 normal;
in vec3 toCam;

uniform sampler2D texture1;

void main()
{
	FragColor = texture(texture1, texCoord);
//    if(FragColor.y > 0.50)
//        FragColor = vec4(1, 1, 1, 0);
//    else
//        FragColor = vec4(0, 0, 0, 0);
}