#version 450 core

struct PointLight {    
    vec3 position;
    vec3 color;
};  

uniform PointLight pointLights[100];
uniform int nLights;
uniform vec3 color;

out vec4 FragColor;

in vec2 texCoord;

in vec3 normal;
in vec3 toCam;
in vec3 position;

uniform sampler2D texture1;
int k = 7, n = 7;

float pick(float u, float v)
{
    float t = texture(texture1, vec2(u, v)).r;

    return t > 0.5 ? 1 : 0;
}

float get(float x, float y, float du, float dv, float w)
{
    float l = 0;
    for (int i = 0; i < k; i++)
    {
        l += pick(x + sin(w + i*2*3.1415/k)*w*du, y + sin(w + i*2*3.1415/k)*w*dv);
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
        sum += get(texCoord.x, texCoord.y, du, dv, 0.7*i*1.0/n);
    }

    sum /= (k*n);

    //float t = smoothstep(0.5 - 0.25, 0.5 + 0.25, FragColor.r);

    FragColor = vec4(color.x, color.y, color.z, 1-sum);
}
