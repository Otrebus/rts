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

void main()
{
    vec3 n = flatShaded ? N_g : N_s;
    if (dot(normalize(tocam), N_g) < 0)
        n = -n; // Make sure the normal is oriented correctly

    vec3 lightDir = vec3(0.5f, -0.3f, -0.4f);
    lightDir = normalize(lightDir);
    float lambertian = max(dot(n, -lightDir), 0.0);

    vec3 ambient = 0.2 * vec3(0.5, 0.5, 0.5);
    vec3 color = vec3(0.5, 0.5, 0.5)*lambertian + ambient;

    color = clamp(color, 0.0, 1.0);

    FragColor = vec4(color, 0);

    vec2 p = texCoord;
    vec2 grid = abs(fract(p-0.5)-0.5)/fwidth(p);
    float line = min(grid.x, grid.y);
    float col = min(line, 1.0);
    FragColor = FragColor - vec4(vec3(col), 0)*0.1;
    if(flatShaded && selected > (1-1e-6))
        FragColor = vec4(0.8, 0, 0, 1);

    for(int i = 0; i < nLights; i++)
    {
        float d = distance(pointLights[i].position, position);
  	    FragColor += vec4(pointLights[i].color, 1)/(0.1 + d*d);
    }
}
