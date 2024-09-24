#version 450 core

struct PointLight {    
    vec3 position;
    vec3 color;
};  

uniform PointLight pointLights[100];
uniform int nLights;

out vec4 FragColor;

in vec2 texCoord;

in vec3 normal;
in vec3 toCam;

uniform sampler2D texture1;
int k = 6, n = 3;

float pick(float u, float v)
{
    float t = texture(texture1, vec2(u, v)).r;
    return smoothstep(0.5 - 0.22, 0.5 + 0.22, t);
}

float get(float x, float y, float du, float dv, float w)
{
    float l = 0;
    for (int i = 0; i < k; i++)
    {
        l += pick(x + sin(i*2*3.1415/k)*w*du, y + sin(i*2*3.1415/k)*w*dv);
    }
    return l;
}

void main()
{
    float du = dFdx(texCoord.x);
    float dv = dFdy(texCoord.y);

    float sum = 0;
    for(int i = 0; i < n; i++)
    {
        sum += get(texCoord.x, texCoord.y, du, dv, i*1.0/n);
    }

    sum /= (k*n);

    //float t = smoothstep(0.5 - 0.25, 0.5 + 0.25, FragColor.r);

    FragColor = vec4(0, 0, 0, 1-sum);
}