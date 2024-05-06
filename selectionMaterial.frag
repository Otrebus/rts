#version 450 core

uniform vec3 Kd;
uniform float radius;
uniform int pass;

in vec3 N_s;
in vec3 N_g; // Updated to use the geometric normal
in vec3 tocam; // Updated to use the toCam vector

in vec2 texCoord;

out vec4 FragColor;

void main()
{
    if(pass == 0)
    {
        vec2 t = texCoord;
        if(t.x*t.x + t.y*t.y < radius*radius)
            FragColor = vec4(Kd, 0);
        else
            discard;

        //vec2 center = vec2(0.5, 0.5);
        //vec2 uv = texCoord;
        //float dist = distance(uv, center);

        //float smoothness = 0.01;

        //float alpha = 1.0 - smoothstep(radius - smoothness, radius + smoothness, dist);

        //FragColor = vec4(alpha, alpha, 0, 1);
    }
    else
    {
        //vec2 center = vec2(0.5, 0.5);
        //vec2 uv = texCoord;
        //float dist = distance(uv, center);

        //float smoothness = 0.01;

        //float alpha = 1.0 - smoothstep(radius - smoothness, radius + smoothness, dist);

        //FragColor = vec4(alpha, 0, 0, alpha);
                vec2 t = texCoord;
        //if((t.x-0.5)*(t.x-0.5) + (t.y-0.5)*(t.y-0.5) < radius) {
        //    FragColor = vec4(1, 0, 0, 1.0f);
        //}
        //else
        //    discard;
    }
}
