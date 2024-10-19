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
    float fx = fract(position.x), fy = fract(position.y);

    if(fogData[y*width+x] == 1)
    {
        FragColor = vec4(0, 0, 0, 1);
        return;
    }

    // should probably be : 1
    int fogR = x < width-1 ? fogData[y*width+x+1] : 0;
    int fogU = y > 0 ? fogData[(y+1)*width+x] : 0;
    int fogL = x > 0 ? fogData[y*width+x-1] : 0;
    int fogD = y < height-1 ? fogData[(y-1)*width+x] : 0;
    int fogUL = (x > 0 && y < height-1) ? fogData[(y+1)*width+x-1] : 0;
    int fogUR = (x < width-1 && y < height - 1) ? fogData[(y+1)*width+x+1] : 0;
    int fogDR = (x < width-1 && y > 0) ? fogData[(y-1)*width+x+1] : 0;
    int fogDL = (x > 0 && y > 0) ? fogData[(y-1)*width+x-1] : 0;

    int fogRR = x < width-2 ? fogData[y*width+x+2] : 0;
    int fogUU = y > 0 ? fogData[(y+2)*width+x] : 0;
    int fogLL = x > 0 ? fogData[y*width+x-2] : 0;
    int fogDD = y < height-2 ? fogData[(y-2)*width+x] : 0;

    float D_a = 0.0, D_b = 0.7;

    // Edges

    if(fogU == 1)
    {
        if(fogR == 0 && fogL == 0)
        {
            if(/*(fogUL == 0 || fogLL == 0) || */ sqrt(2.0)*fx > (1-fy) && (/*fogUR == 0 || fogRR == 0 || */ sqrt(2.0)*(1-fx) > (1-fy)))
            {
                float t = smoothstep(D_a, D_b, 1.0-fract(position.y));
                FragColor = FragColor*vec4(t, t, t, 1);
            }
        }
        if(fogR == 1)
        {
            float r = sqrt(2.0)*((1-fx) - fy)/2;
            float t = smoothstep(D_a, D_b, r);
            FragColor = FragColor*vec4(t, t, t, 1);
        }
    }
    if(fogL == 1)
    {
        if(fogU == 0 && fogD == 0)
        {
            if((fogUL == 0 || fogUU == 0 || fy < 1-fx*(sqrt(2.0)-1)) && (fogDL == 0 || fogDD == 0 || fy > fx*(sqrt(2.0)-1)))
            {
                float t = smoothstep(D_a, D_b, fract(position.x));
                FragColor = FragColor*vec4(t, t, t, 1);
            }
        }
        if(fogU == 1)
        {
            float r = sqrt(2.0)*(fx - fy)/2;
            float t = smoothstep(D_a, D_b, r);
            FragColor = FragColor*vec4(t, t, t, 1);
        }
    }
    if(fogD == 1)
    {
        if(fogR == 0 && fogL == 0)
        {
            if((fogDL == 0 || fogLL == 0 || fx > (sqrt(2.0)-1)*fy) && (fx < 1-fy*(sqrt(2.0)-1) || fogDR == 0 || fogRR == 0))
            {
                float t = smoothstep(D_a, D_b, fract(position.y));
                FragColor = FragColor*vec4(t, t, t, 1);
            }
        }
        if(fogL == 1)
        {
            float r = sqrt(2.0)*(fy + fx - 1)/2;
            float t = smoothstep(D_a, D_b, r);
            FragColor = FragColor*vec4(t, t, t, 1);
        }
    }
    if(fogR == 1)
    {
        if(fogU == 0 && fogD == 0)
        {
            //float X = 1 - D_b, Y = 1 - D_b - (1-sqrt(2.0)*D_b);
            if((fogDR == 0 || fogDD == 0 || fy > (1-sqrt(2.0))*(fx-1)) && (fogUR == 0 || fogUU == 0 || fy < 1-(1-sqrt(2.0))*(fx-1)))
            {
                float t = smoothstep(D_a, D_b, 1-fract(position.x));
                FragColor = FragColor*vec4(t, t, t, 1);
            }
        }
        if(fogD == 1)
        {
            float r = sqrt(2.0)*(fy-fx)/2;
            float t = smoothstep(D_a, D_b, r);
            FragColor = FragColor*vec4(t, t, t, 1);
        }        
    }

    // Corners

    if(fogUR == 1 && fogR == 0 && fogU == 0)
    {
        if(fx > fy)
        {
            if(fogRR == 0)
            {
                float r = distance(vec2(1.0, 1.0), vec2(fx, fy));
                float t = smoothstep(D_a, D_b, r);
                FragColor = FragColor*vec4(t, t, t, 1);
            }
            else
            {
                float r = sqrt(2.0)*((1-fx) - (fy-1))/2;
                float t = smoothstep(D_a, D_b, r);
                FragColor = FragColor*vec4(t, t, t, 1);                 
            }
        }
        if(fy > fx)
        {
            if(fogUU == 0)
            {
                float r = distance(vec2(1.0, 1.0), vec2(fx, fy));
                float t = smoothstep(D_a, D_b, r);
                FragColor = FragColor*vec4(t, t, t, 1);
            }
            else
            {
                float r = sqrt(2.0)*((1-fx) - (fy-1))/2;
                float t = smoothstep(D_a, D_b, r);
                FragColor = FragColor*vec4(t, t, t, 1);
            }
        }
    }
    if(fogUL == 1 && fogU == 0 && fogL == 0)
    {
        if(fy < 1-fx)
        {
            if(fogLL == 0)
            {
                float r = distance(vec2(0.0, 1.0), vec2(fx, fy));
                float t = smoothstep(D_a, D_b, r);
                FragColor = FragColor * vec4(t, t, t, 1);
            }
            else
            {
                float r = sqrt(2.0)*(1+fx - fy)/2;
                float t = smoothstep(D_a, D_b, r);
                FragColor = FragColor * vec4(t, t, t, 1);                 
            }
        }
        if(fy > 1-fx)
        {
            if(fogUU == 0)
            {
                float r = distance(vec2(0.0, 1.0), vec2(fx, fy));
                float t = smoothstep(D_a, D_b, r);
                FragColor = FragColor * vec4(t, t, t, 1);
            }
            else
            {
                float r = sqrt(2.0)*((fx) - (fy - 1)) / 2;
                float t = smoothstep(D_a, D_b, r);
                FragColor = FragColor * vec4(t, t, t, 1);
            }
        }
    }
    if(fogDR == 1 && fogR == 0 && fogD == 0)
    {
        if(fy > 1-fx)
        {
            if(fogRR == 0)
            {
                float r = distance(vec2(1.0, 0.0), vec2(fx, fy));
                float t = smoothstep(D_a, D_b, r);
                FragColor = FragColor*vec4(t, t, t, 1);
            }
            else
            {
                float r = sqrt(2.0)*(fy-(fx-1))/2;
                float t = smoothstep(D_a, D_b, r);
                FragColor = FragColor*vec4(t, t, t, 1);                 
            }
        }
        if(fy < 1-fx)
        {
            if(fogDD == 0)
            {
                float r = distance(vec2(1.0, 0.0), vec2(fx, fy));
                float t = smoothstep(D_a, D_b, r);
                FragColor = FragColor*vec4(t, t, t, 1);
            }
            else
            {
                float r = sqrt(2.0)*(fy-(fx-1))/2;
                float t = smoothstep(D_a, D_b, r);
                FragColor = FragColor*vec4(t, t, t, 1);
            }
        }
    }
    if(fogDL == 1 && fogL == 0 && fogD == 0)
    {
        if(fy > fx)
        {
            if(fogLL == 0)
            {
                float r = distance(vec2(0.0, 0.0), vec2(fx, fy));
                float t = smoothstep(D_a, D_b, r);
                FragColor = FragColor*vec4(t, t, t, 1);
            }
            else
            {
                float r = sqrt(2.0)*(fy + fx)/2;
                float t = smoothstep(D_a, D_b, r);
                FragColor = FragColor*vec4(t, t, t, 1);                 
            }
        }
        if(fy < fx)
        {
            if(fogDD == 0)
            {
                float r = distance(vec2(0.0, 0.0), vec2(fx, fy));
                float t = smoothstep(D_a, D_b, r);
                FragColor = FragColor*vec4(t, t, t, 1);
            }
            else
            {
                float r = sqrt(2.0)*(fy + fx)/2;
                float t = smoothstep(D_a, D_b, r);
                FragColor = FragColor*vec4(t, t, t, 1);                 
            }
        }
    }

    if(fogDL == 1 && fogD == 1 && fogLL == 1)
    {
        if(-fx > (1-sqrt(2.0))*fy)
        {
            float r = sqrt(2.0)*(fy + fx)/2;
            float t = smoothstep(D_a, D_b, r);
            FragColor = FragColor*vec4(t, t, t, 1);
        }
    }
    if(fogDR == 1 && fogD == 1 && fogRR == 1)
    {
        if(fx > 1-fy*(sqrt(2.0)-1))
        {
            float r = sqrt(2.0)*(fy-(fx-1))/2;
            float t = smoothstep(D_a, D_b, r);
            FragColor = FragColor*vec4(t, t, t, 1);
        }
    }

    if(fogUR == 1 && fogR == 1 && fogUU == 1)
    {
        if(fy > 1-(1-sqrt(2.0))*(fx-1))
        {
            float r = sqrt(2.0)*((1-fx) - (fy-1))/2;
            float t = smoothstep(D_a, D_b, r);
            FragColor = FragColor*vec4(t, t, t, 1);
        }
    }
    if(fogDR == 1 && fogR == 1 && fogDD == 1)
    {
        if(fy < (1-sqrt(2.0))*(fx-1))
        {
            float r = sqrt(2.0)*(fy+1-fx)/2;
            float t = smoothstep(D_a, D_b, r);
            FragColor = FragColor*vec4(t, t, t, 1);
        }
    }

    if(fogUR == 1 && fogU == 1 && fogRR == 1)
    {
        float r = sqrt(2.0)*((1-fx) - (fy-1))/2;
        float t = smoothstep(D_a, D_b, r);
        FragColor = FragColor*vec4(t, t, t, 1);
    }
    if(fogUL == 1 && fogU == 1 && fogLL == 1)
    {
        if(sqrt(2.0)*fx < (1-fy))
        {
            float r = sqrt(2.0)*(fx - fy + 1)/2;
            float t = smoothstep(D_a, D_b, r);
            FragColor = FragColor*vec4(t, t, t, 1);
        }
    }

    if(fogUL == 1 && fogL == 1 && fogUU == 1)
    {
        if(fy > 1-fx*(sqrt(2.0)-1))
        {
            float r = sqrt(2.0)*(fx - (fy-1))/2;
            float t = smoothstep(D_a, D_b, r);
            FragColor = FragColor*vec4(t, t, t, 1);
        }
    }
    if(fogDL == 1 && fogL == 1 && fogDD == 1)
    {
        if(fy < fx*(sqrt(2.0)-1))
        {
            float r = sqrt(2.0)*(fy + fx)/2;
            float t = smoothstep(D_a, D_b, r);
            FragColor = FragColor*vec4(t, t, t, 1);
        }
    }
}
