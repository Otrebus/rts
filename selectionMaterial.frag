#version 450 core


uniform vec3 Kd;
uniform float radius;
uniform int pass;

in vec2 texCoord;

out vec4 FragColor;

void main()
{
    vec2 t = texCoord;
    if(t.x*t.x + t.y*t.y < radius*radius)
        FragColor = vec4(Kd, 0.5);
    else
        discard;
}
