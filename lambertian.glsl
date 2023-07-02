#version 450 core

uniform vec3 Kd;
in vec3 normal;
in vec3 toCam;

out vec4 FragColor;

void main()
{
    vec3 n = normal;
    normalize(toCam);
    if(dot(toCam, n) < 0)
        n = -normal;

    vec3 lightDir = vec3(0.5f, -0.3f, -0.4f);
    normalize(lightDir);
    float lambertian = max(dot(n, -lightDir), 0.0);
    vec3 color = Kd * lambertian;
    FragColor = vec4(color, 1.0f);
}
