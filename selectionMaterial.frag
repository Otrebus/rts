#version 450 core

uniform vec3 Kd;

in vec3 N_s;
in vec3 N_g; // Updated to use the geometric normal
in vec3 tocam; // Updated to use the toCam vector

in vec2 texCoord;

out vec4 FragColor;

void main()
{
    vec2 t = texCoord;
    if((t.x-0.5)*(t.x-0.5) + (t.y-0.5)*(t.y-0.5) < 0.2)
        FragColor = vec4(1, 0, 0, 1.0f);
    else
        FragColor = vec4(0, 1, 0, 1.0f);
}
