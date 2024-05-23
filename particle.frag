#version 450 core

in vec3 color;
in vec2 texCoord;

out vec4 fragColor;

void main()
{
    float radius = 0.5;
    vec2 t = texCoord;
    if((t.x-0.5)*(t.x-0.5) + (t.y-0.5)*(t.y-0.5) > radius*radius)
        discard;
    else
        fragColor = vec4(color, 1.0);
}