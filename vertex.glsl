#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
layout (location = 2) in vec2 aTex;
//uniform float u_time;
out vec3 test;
out vec3 col;
out vec2 tex;

void main()
{
	//float x = aPos.x + 0.1*cos(u_time);
    float x = 1.0;
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    col = aCol;
    tex = aTex;
}
