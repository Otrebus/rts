#version 330 core
out vec4 FragColor;
uniform vec3 Kd;

void main() {
    FragColor = vec4(Kd, 1.0f);
}