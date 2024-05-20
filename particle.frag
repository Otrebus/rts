#version 450 core

//in vec2 texCoord;
in vec4 color;

out vec4 fragColor;

uniform sampler2D particleTexture;

void main()
{
    //vec4 textureColor = texture(particleTexture, texCoord);
    //fragColor = color * textureColor;
    fragColor = color;
}