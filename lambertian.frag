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
in vec3 position;

out vec4 FragColor;

void main()
{
    vec3 n = N_s;
    if (dot(normalize(tocam), N_g) < 0)
        n = -N_s; // Make sure the normal is oriented correctly

    vec3 lightDir = vec3(0.5f, -0.3f, -0.4f);
    lightDir = normalize(lightDir);
    float lambertian = max(dot(n, -lightDir), 0.0);

    vec3 ambient = 0.2 * Kd;
    vec3 color = Kd * lambertian + ambient;

    color = clamp(color, 0.0, 1.0);
    FragColor = vec4(color, 0.0);

    for(int i = 0; i < nLights; i++)
    {
        lightDir = normalize(pointLights[i].position-position);
        lambertian = max(dot(n, lightDir), 0.0);
        float d = distance(pointLights[i].position, position);
  	    FragColor += lambertian*vec4(pointLights[i].color, 1)/(d*d);
    }

}