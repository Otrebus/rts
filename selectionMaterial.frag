#version 450 core

uniform vec3 Kd;
uniform float radius;
uniform int pass;
uniform float alpha;

in vec2 texCoord;

out vec4 FragColor;

void main()
{
    vec2 t = texCoord;
    if(t.x*t.x + t.y*t.y < radius*radius)
        FragColor = vec4(Kd, alpha);
    else
        discard;
}
