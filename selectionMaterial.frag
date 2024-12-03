#version 450 core

uniform vec3 Kd;
uniform float radius;
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
        if(t.x*t.x + t.y*t.y < radius*radius)
            FragColor = vec4(Kd, alpha);
        else
            discard;
    }
    else
    {
        vec2 t = texCoord - vec2(float(length)/2, float(width)/2);
        
        if(t.x < radius && t.y > width-radius)
        {
            if(distance(vec2(t.x, t.y), vec2(radius, width-radius)) < radius)
                FragColor = vec4(Kd, alpha);
            else
                discard;
        }
        else if(t.x > length-radius && t.y > width-radius)
        {
            if(distance(vec2(t.x, t.y), vec2(length-radius, width-radius)) < radius)
                FragColor = vec4(Kd, alpha);
            else
                discard;
        }
        else if(t.x < radius && t.y < radius)
        {
            if(distance(vec2(t.x, t.y), vec2(radius, radius)) < radius)
                FragColor = vec4(Kd, alpha);
            else
                discard;
        }
        else if(t.x > length-radius && t.y < radius)
        {
            if(distance(vec2(t.x, t.y), vec2(length-radius, radius)) < radius)
                FragColor = vec4(Kd, alpha);
            else
                discard;
        }
        else if(t.x > radius && t.x < float(length)-radius && t.y > radius && t.y < float(width)-radius)
            FragColor = vec4(Kd, alpha);
        else
            discard;
    }
    else
        discard;
}
