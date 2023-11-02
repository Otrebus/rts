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

    FragColor = vec4(lambertian, lambertian, lambertian, 1.0);
    vec3 color = Kd*lambertian;
    FragColor = vec4(color, 1.0f);
    //FragColor = vec4(normal.x, normal.y, normal.z, 1.0f);
    //FragColor = vec4(normal, 1.0);
}
