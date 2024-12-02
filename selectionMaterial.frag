#version 450 core

uniform vec3 Kd;
uniform float radius;
uniform int pass;
uniform float alpha;
uniform int width, height, circular;

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
        vec2 t = texCoord;
        if(t.x < 0.2 || t.x > width-0.2 || t.y < 0.2 || t.y > height-0.2)
            FragColor = vec4(t.x, t.x, t.x, alpha);
        else
            discard;
    }
}
