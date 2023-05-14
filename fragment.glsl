#version 450 core

out vec4 FragColor;
in vec3 col;
in vec2 tex;

uniform sampler2D text;

void main()
{
    //FragColor = vec4(tex, 1.0f);
    FragColor = texture(text, tex);
}
