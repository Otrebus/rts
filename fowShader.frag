#version 450 core

in vec3 position;

out vec4 FragColor;


layout(std430, binding = 0) buffer FogDataBuffer {
    int width;
    int height;
    int fogData[];
};

void main()
{
    int x = int(position.x), y = int(position.y);
    float fx = fract(position.x), fy = fract(position.y);

    float fowAlpha = 0.3;

    if(fogData[y*width+x] == 1)
    {
        FragColor = vec4(1.0-fowAlpha, 1.0-fowAlpha, 1.0-fowAlpha, 0);
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

    float t = 1;

    // Edges
    if(fogU == 1)
    {
        // Edge is horizontal
        if(fogR == 0 && fogL == 0)
        {
            // Don't go all the way if we have an inner 45 degree edge
            if((fogUL == 0 || fogLL == 0 || fx > (sqrt(2.0)-1.0)*(1-fy)) && (fogUR == 0 || fogRR == 0 || -fx > -1+(sqrt(2.0)-1.0)*(1-fy)))
            {
                t *= smoothstep(D_a, D_b, 1.0-fract(position.y));
            }
        }

        // Edge is diagonal edge
        if(fogR == 1)
        {
            float r = sqrt(2.0)*((1-fx) - fy)/2;
            t *= smoothstep(D_a, D_b, r);
        }
    }
    if(fogL == 1)
    {
        if(fogU == 0 && fogD == 0)
        {
            if((fogUL == 0 || fogUU == 0 || fy < 1-fx*(sqrt(2.0)-1)) && (fogDL == 0 || fogDD == 0 || fy > fx*(sqrt(2.0)-1)))
            {
                t *= smoothstep(D_a, D_b, fract(position.x));
            }
        }
        if(fogU == 1)
        {
            float r = sqrt(2.0)*(fx - fy)/2;
            t *= smoothstep(D_a, D_b, r);
        }
    }
    if(fogD == 1)
    {
        if(fogR == 0 && fogL == 0)
        {
            if((fogDL == 0 || fogLL == 0 || fx > (sqrt(2.0)-1)*fy) && (fx < 1-fy*(sqrt(2.0)-1) || fogDR == 0 || fogRR == 0))
            {
                t *= smoothstep(D_a, D_b, fract(position.y));
            }
        }
        if(fogL == 1)
        {
            float r = sqrt(2.0)*(fy + fx - 1)/2;
            t *= smoothstep(D_a, D_b, r);
        }
    }
    if(fogR == 1)
    {
        if(fogU == 0 && fogD == 0)
        {
            //float X = 1 - D_b, Y = 1 - D_b - (1-sqrt(2.0)*D_b);
            if((fogDR == 0 || fogDD == 0 || fy > (1-sqrt(2.0))*(fx-1)) && (fogUR == 0 || fogUU == 0 || fy < 1-(1-sqrt(2.0))*(fx-1)))
            {
                t *= smoothstep(D_a, D_b, 1-fract(position.x));
            }
        }
        if(fogD == 1)
        {
            float r = sqrt(2.0)*(fy-fx)/2;
            t *= smoothstep(D_a, D_b, r);
        }        
    }

    // Outer corners
    if(fogUR == 1 && fogR == 0 && fogU == 0)
    {
        // Half the corner
        if(fx > fy)
        {
            // 90 degree bend
            if(fogRR == 0)
            {
                float r = distance(vec2(1.0, 1.0), vec2(fx, fy));
                t *= smoothstep(D_a, D_b, r);
            }
            // 45 degree bend
            else
            {
                float r = sqrt(2.0)*((1-fx) - (fy-1))/2;
                t *= smoothstep(D_a, D_b, r);
            }
        }
        if(fy > fx)
        {
            if(fogUU == 0)
            {
                float r = distance(vec2(1.0, 1.0), vec2(fx, fy));
                t *= smoothstep(D_a, D_b, r);
            }
            else
            {
                float r = sqrt(2.0)*((1-fx) - (fy-1))/2;
                t *= smoothstep(D_a, D_b, r);
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
                t *= smoothstep(D_a, D_b, r);
            }
            else
            {
                float r = sqrt(2.0)*(1+fx - fy)/2;
                t *= smoothstep(D_a, D_b, r);
            }
        }
        if(fy > 1-fx)
        {
            if(fogUU == 0)
            {
                float r = distance(vec2(0.0, 1.0), vec2(fx, fy));
                t *= smoothstep(D_a, D_b, r);
            }
            else
            {
                float r = sqrt(2.0)*((fx) - (fy - 1)) / 2;
                t *= smoothstep(D_a, D_b, r);
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
                t *= smoothstep(D_a, D_b, r);
            }
            else
            {
                float r = sqrt(2.0)*(fy-(fx-1))/2;
                t *= smoothstep(D_a, D_b, r);
            }
        }
        if(fy < 1-fx)
        {
            if(fogDD == 0)
            {
                float r = distance(vec2(1.0, 0.0), vec2(fx, fy));
                t *= smoothstep(D_a, D_b, r);
            }
            else
            {
                float r = sqrt(2.0)*(fy-(fx-1))/2;
                t *= smoothstep(D_a, D_b, r);
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
                t *= smoothstep(D_a, D_b, r);
            }
            else
            {
                float r = sqrt(2.0)*(fy + fx)/2;
                t *= smoothstep(D_a, D_b, r);
            }
        }
        if(fy < fx)
        {
            if(fogDD == 0)
            {
                float r = distance(vec2(0.0, 0.0), vec2(fx, fy));
                t *= smoothstep(D_a, D_b, r);
            }
            else
            {
                float r = sqrt(2.0)*(fy + fx)/2;
                t *= smoothstep(D_a, D_b, r);
            }
        }
    }

    // Inner 45 degree edges    
    if(fogDL == 1 && fogD == 1 && fogLL == 1 && -fx > (1-sqrt(2.0))*fy)
    {
        float r = sqrt(2.0)*(fy + fx)/2;
        t *= smoothstep(D_a, D_b, r);
    }
    if(fogDR == 1 && fogD == 1 && fogRR == 1 && fx > 1-fy*(sqrt(2.0)-1))
    {
        float r = sqrt(2.0)*(fy-(fx-1))/2;
        t *= smoothstep(D_a, D_b, r);
    }

    if(fogUR == 1 && fogR == 1 && fogUU == 1 && fy > 1-(1-sqrt(2.0))*(fx-1))
    {
        float r = sqrt(2.0)*((1-fx) - (fy-1))/2;
        t *= smoothstep(D_a, D_b, r);
    }
    if(fogDR == 1 && fogR == 1 && fogDD == 1 && fy < (1-sqrt(2.0))*(fx-1))
    {
        float r = sqrt(2.0)*(fy+1-fx)/2;
        t *= smoothstep(D_a, D_b, r);
    }
    if(fogUR == 1 && fogU == 1 && fogRR == 1 && -fx < -1+(sqrt(2.0)-1.0)*(1-fy))
    {
        float r = sqrt(2.0)*((1-fx) - (fy-1))/2;
        t *= smoothstep(D_a, D_b, r);
    }
    if(fogUL == 1 && fogU == 1 && fogLL == 1 && fx < (sqrt(2.0)-1.0)*(1-fy))
    {
        float r = sqrt(2.0)*(fx - fy + 1)/2;
        t *= smoothstep(D_a, D_b, r);
    }
    if(fogUL == 1 && fogL == 1 && fogUU == 1 && fy > 1-fx*(sqrt(2.0)-1))
    {
        float r = sqrt(2.0)*(fx - (fy-1))/2;
        t *= smoothstep(D_a, D_b, r);
    }
    if(fogDL == 1 && fogL == 1 && fogDD == 1 && fy < fx*(sqrt(2.0)-1))
    {
        float r = sqrt(2.0)*(fy + fx)/2;
        t *= smoothstep(D_a, D_b, r);
    }
    FragColor = vec4(1-fowAlpha+fowAlpha*t, 1-fowAlpha+fowAlpha*t, 1-fowAlpha+fowAlpha*t, max(0, min(1, t)) );
}
