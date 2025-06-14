#version 450 core

in vec4 color;
in vec2 texCoord;

out vec4 fragColor;

void main()
{
    float radius = 0.3;
    vec2 t = texCoord;
    float d = (t.x-0.5)*(t.x-0.5) + (t.y-0.5)*(t.y-0.5);
    if(d > radius*radius)
        fragColor = vec4(color.xyz, color.w*(1-10*(d-radius*radius)));
    else
        fragColor = color;
}
