#version 450 core

struct PointLight {    
    vec3 position;
    vec3 color;
};  

uniform PointLight pointLights[100];
uniform int nLights;

in vec3 N_s;
in vec3 N_g; // Updated to use the geometric normal
in vec3 tocam; // Updated to use the toCam vector
in vec3 position;

in vec2 texCoord;
out vec4 FragColor;

uniform int terrainWidth;
uniform int terrainHeight;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

layout(std430, binding = 0) buffer AdmissibleDataBuffer {
    int width;
    int height;
    int admissibleData[];
};

vec4 hash4( vec2 p )
{
  return fract(
    sin(vec4(1.0+dot(p,vec2(37.0,17.0)),
              2.0+dot(p,vec2(11.0,47.0)),
              3.0+dot(p,vec2(41.0,29.0)),
              4.0+dot(p,vec2(23.0,31.0))))*103.0);
}

    float sum( vec3 v ) { return v.x+v.y+v.z; }

vec4 textureNoTile( sampler2D samp, in vec2 uv ) // from https://iquilezles.org/articles/texturerepetition/
{
    ivec2 iuv = ivec2( floor( uv ) );
     vec2 fuv = fract( uv );

    // generate per-tile transform
    vec4 ofa = hash4( iuv + ivec2(0,0) );
    vec4 ofb = hash4( iuv + ivec2(1,0) );
    vec4 ofc = hash4( iuv + ivec2(0,1) );
    vec4 ofd = hash4( iuv + ivec2(1,1) );
    
    vec2 ddx = dFdx( uv );
    vec2 ddy = dFdy( uv );

    // transform per-tile uvs
    ofa.zw = sign( ofa.zw-0.5 );
    ofb.zw = sign( ofb.zw-0.5 );
    ofc.zw = sign( ofc.zw-0.5 );
    ofd.zw = sign( ofd.zw-0.5 );
    
    // uv's, and derivatives (for correct mipmapping)
    vec2 uva = uv*ofa.zw + ofa.xy, ddxa = ddx*ofa.zw, ddya = ddy*ofa.zw;
    vec2 uvb = uv*ofb.zw + ofb.xy, ddxb = ddx*ofb.zw, ddyb = ddy*ofb.zw;
    vec2 uvc = uv*ofc.zw + ofc.xy, ddxc = ddx*ofc.zw, ddyc = ddy*ofc.zw;
    vec2 uvd = uv*ofd.zw + ofd.xy, ddxd = ddx*ofd.zw, ddyd = ddy*ofd.zw;
        
    // fetch and blend
    vec2 b = smoothstep( 0.25,0.75, fuv );
    
    return mix( mix( textureGrad( samp, uva, ddxa, ddya ), 
                     textureGrad( samp, uvb, ddxb, ddyb ), b.x ), 
                mix( textureGrad( samp, uvc, ddxc, ddyc ),
                     textureGrad( samp, uvd, ddxd, ddyd ), b.x), b.y );
}

void main()
{
    vec3 n = N_s;
    if (dot(normalize(tocam), N_g) < 0)
        n = -n; // Make sure the normal is oriented correctly

    vec3 lightDir = vec3(0.5f, -0.3f, -0.4f);
    lightDir = normalize(lightDir);
    float lambertian = max(dot(n, -lightDir), 0.0);

    vec3 ambient = vec3(0.1, 0.1, 0.14);
    vec3 color = vec3(0.8, 0.75, 0.7)*lambertian + ambient;

//	vec4 FragColor1 = textureNoTile(texture1, texCoord);
//    vec4 FragColor2 = textureNoTile(texture2, texCoord);
    vec4 splatMap = texture(texture3, vec2(texCoord.x/terrainWidth, texCoord.y/terrainHeight));

    vec4 texSum = vec4(0, 0, 0, 0);
    /*
    if(splatMap.r > 0) 
        texSum += splatMap.r*textureNoTile(texture1, texCoord);
    if(splatMap.g > 0)
        texSum += splatMap.g*textureNoTile(texture2, texCoord);
    if(splatMap.b > 0)
        texSum += splatMap.b*textureNoTile(texture2, texCoord);
    */

    if(splatMap.r > 0) 
        texSum += splatMap.r*texture(texture1, texCoord);
    if(splatMap.g > 0)
        texSum += splatMap.g*texture(texture2, texCoord);
    if(splatMap.b > 0)
        texSum += splatMap.b*texture(texture2, texCoord);

    color = clamp(color, 0.0, 1.0);
    FragColor = texSum*(vec4(color, 1));

    for(int i = 0; i < nLights; i++)
    {
        lightDir = normalize(pointLights[i].position-position);
        lambertian = max(dot(n, lightDir), 0.0);
        float d = distance(pointLights[i].position, position);
  	    FragColor += lambertian*vec4(pointLights[i].color, 1)/(0.1+0.2*d*d);
    }
}
