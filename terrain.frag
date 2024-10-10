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


layout(std430, binding = 0) buffer FogDataBuffer {
    int width;
    int height;
    int fogData[];
};

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
        FragColor = vec4(0.8, 0, 0, 1)*(0.2+0.8*lambertian);

    for(int i = 0; i < nLights; i++)
    {
        float d = distance(pointLights[i].position, position);
  	    FragColor += vec4(pointLights[i].color, 1)/(0.1 + d*d);
    }
    FragColor.w = 1;

    int x = int(position.x), y = int(position.y);

    if(fogData[y*width+x] == 1)
    {
        FragColor = vec4(0, 0, 0, 1);
        return;
    }

    int fogR = x < width-1 ? fogData[y*width+x+1] : 0;
    int fogU = y > 0 ? fogData[(y+1)*width+x] : 0;
    int fogL = x > 0 ? fogData[y*width+x-1] : 0;
    int fogD = y < height-1 ? fogData[(y-1)*width+x] : 0;

    if(fogR == 1)
    {
        if(fract(position.x) > 0.5 && fract(position.y) > 0.5)
        {
            if(fogU == 1)
            {
                float r = fract(distance(vec2(0.5, 0.5), vec2(fract(position.x), fract(position.y))));
                float t = smoothstep(0.3, 0.5, r);
                FragColor = FragColor*vec4(1-t, 1-t, 1-t, 1);
            }
        }
        else
        {
            float t = smoothstep(0.8, 1.0, fract(position.x));
            FragColor = FragColor*vec4(1-t, 1-t, 1-t, 1);
        }
    }
    if(fogU == 1)
    {
        float t = smoothstep(0.8, 1.0, fract(position.y));
        FragColor = FragColor*vec4(1-t, 1-t, 1-t, 1);
    }
    if(fogL == 1)
    {
        float t = smoothstep(0.0, 0.2, fract(position.x));
        FragColor = FragColor*vec4(t, t, t, 1);
    }
    if(fogD == 1)
    {
        float t = smoothstep(0.0, 0.2, fract(position.y));
        FragColor = FragColor*vec4(t, t, t, 1);
    }
}
