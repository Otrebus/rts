#version 450 core

uniform vec3 Kd;

in vec2 TexCoord;

in vec3 normal;
in vec3 toCam;

out vec4 FragColor;

void main()
{
    vec3 n = normal;
    if(dot(normalize(toCam), n) < 0)
        n = -normal;

    vec3 lightDir = vec3(0.5f, -0.3f, -0.4f);
    lightDir = normalize(lightDir);
    float lambertian = max(dot(n, -lightDir), 0.0);

    vec3 ambient = 0.2*vec3(Kd.x, Kd.y, Kd.z);
    vec3 color = Kd*lambertian + ambient;

    color = clamp(color, 0.0, 1.0);

    FragColor = vec4(color, 1.0f);
}
