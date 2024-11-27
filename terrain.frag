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

float P = 0.1; // Area overflow of colored admissible areas


layout(std430, binding = 0) buffer FogDataBuffer {
    int width;
    int height;
    int fogData[];
};

layout(std430, binding = 1) buffer AdmissibleDataBuffer {
    int admissibleWidth;
    int admissibleHeight;
    int admissibleData[];
};

bool getT(vec2 p)
{
    float y = fract(p.y);
    float x = fract(p.x);

    int Y = int(p.y);
    int X = int(p.x);

    bool tr = admissibleData[width*(Y+1)+(X+1)] == 0;
    bool tl = admissibleData[width*(Y+1)+X] == 0;
    bool br = admissibleData[width*Y+X+1] == 0;
    bool bl = admissibleData[width*Y+X] == 0;

    bool u = tr && tl;
    bool l = tl && bl;
    bool r = tr && br;
    bool d = bl && br;

    bool b = false;

    
    if(tr && bl && tl && br)
    {
        return true;
    }
    else if(tl && tr && bl)
    {
        return y > x - P*sqrt(2.0);
    }
    else if(tl && tr && br)
    {
        return y > 1-x - P*sqrt(2.0);
    }
    else if(tr && br && bl)
    {
        return y < x + P*sqrt(2.0);
    }
    else if(br && bl && tl)
    {
        return y < 1 - x + P*sqrt(2.0);
    }
    else if(u && !d)
    {
        return y > 1-P;
    }
    else if(d && !u)
    {
        return y < P;
    }
    else if(u && d)
    {
        return y < P && y > 1-P;
    }
    else if(l && !r)
    {
        return x < P;
    }
    else if(r && !l)
    {
        return x > 1-P;
    }
    else if(r && l)
    {
        return x > 1-P;
    }
    else if(tr)
    {
        return distance(vec2(x, y), vec2(1, 1)) < P;
    }
    else if(tl)
    {
        return distance(vec2(x, y), vec2(0, 1)) < P;
    }
    else if(bl)
    {
        return distance(vec2(x, y), vec2(0, 0)) < P;
    }
    else if(br)
    {
        return distance(vec2(x, y), vec2(1, 0)) < P;
    }
    return false;
}

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
//    if(flatShaded && selected > (1-1e-6))
//        FragColor = vec4(0.8, 0, 0, 1)*(0.2+0.8*lambertian);

    for(int i = 0; i < nLights; i++)
    {
        float d = distance(pointLights[i].position, position);
  	    FragColor += vec4(pointLights[i].color, 1)/(0.1 + d*d);
    }

    if(flatShaded)
    {
        if(getT(p))
            FragColor = vec4(0.8, 0, 0, 1);    
        int x = int(p.x);
        int y = int(p.y);
        if(admissibleData[width*y+x] == 0 && distance(vec2(p.x, p.y), vec2(x, y)) < P)
            FragColor = vec4(0.8, 0, 0, 1)*(0.2+0.8*lambertian);
        if(admissibleData[width*(y+1)+x] == 0 && distance(vec2(p.x, p.y), vec2(x, y+1)) < P)
            FragColor = vec4(0.8, 0, 0, 1)*(0.2+0.8*lambertian);
        if(admissibleData[width*y+x+1] == 0 && distance(vec2(p.x, p.y), vec2(x+1, y)) < P)
            FragColor = vec4(0.8, 0, 0, 1)*(0.2+0.8*lambertian);
        if(admissibleData[width*(y+1)+x+1] == 0 && distance(vec2(p.x, p.y), vec2(x+1, y+1)) < P)
            FragColor = vec4(0.8, 0, 0, 1)*(0.2+0.8*lambertian);
    }

    FragColor.w = 1;
}
