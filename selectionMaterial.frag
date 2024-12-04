#version 450 core

uniform vec3 Kd;
uniform float radiusA;
uniform float radiusB;
uniform int pass;
uniform float alpha;
uniform int length, width, circular;

in vec2 texCoord;

out vec4 FragColor;

void main()
{
    if(circular == 1)
    {
        vec2 t = texCoord;
        if(t.x*t.x + t.y*t.y < radiusA*radiusA)
            FragColor = vec4(Kd, alpha);
        else
            discard;
    }
    else
    {
        // TODO: I bet stuff like this would be simpler if we used some distance field approach
        vec2 t = texCoord;
        float L = float(length);
        float W = float(width);
        float R = 0.2;
        float ir = 1.0-radiusA;
        
        if(t.x < -radiusA+R && t.y > radiusB-R)
        {
            if(distance(vec2(t.x, t.y), vec2(-radiusA+R, radiusB-R)) < R)
                FragColor = vec4(Kd, alpha);
            else
                discard;
        }
        else if(t.x > radiusA-R && t.y > radiusB-R)
        {
            if(distance(vec2(t.x, t.y), vec2(radiusA-R, radiusB-R)) < R)
                FragColor = vec4(Kd, alpha);
            else
                discard;
        }
        else if(t.x > radiusA-R && t.y < -radiusB+R)
        {
            if(distance(vec2(t.x, t.y), vec2(radiusA-R, -radiusB+R)) < R)
                FragColor = vec4(Kd, alpha);
            else
                discard;
        }
        else if(t.x < -radiusA+R && t.y < -radiusB+R)
        {
            if(distance(vec2(t.x, t.y), vec2(-radiusA+R, -radiusB+R)) < R)
                FragColor = vec4(Kd, alpha);
            else
                discard;
        }
        else if(t.x > -radiusA && t.x < radiusA && t.y > -radiusB && t.y < radiusB)
            FragColor = vec4(Kd, alpha);
        else
            discard;
    }
}
