#version 450 core

struct PointLight {    
    vec3 position;
    vec3 color;
};  

uniform PointLight pointLights[100];
uniform int nLights;

in vec3 N_s;
in vec3 N_g; // Updated to use the geometric normal
in vec3 tocam; // Updated to use the toCam vector
in vec3 position;

in vec2 texCoord;
out vec4 FragColor;
flat in int selected;

uniform bool flatShaded = false;

uniform sampler2D texture1;

void main()
{
    vec3 n = N_s;
    if (dot(normalize(tocam), N_g) < 0)
        n = -n; // Make sure the normal is oriented correctly

    vec3 lightDir = vec3(0.5f, -0.3f, -0.4f);
    lightDir = normalize(lightDir);
    float lambertian = max(dot(n, -lightDir), 0.0);

    vec3 ambient = vec3(0.01, 0.01, 0.07);
    vec3 color = vec3(0.9, 0.85, 0.8)*lambertian + ambient;

    color = clamp(color, 0.0, 1.0);
	FragColor = texture(texture1, texCoord)*(vec4(color, 1) + vec4(ambient, 1));
}
