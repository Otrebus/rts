#include "TextureMaterial.h"
#include "Vector3.h"
#include "Shader.h"


TextureMaterial::TextureMaterial(const std::string& textureFile)
{
    if(!shader) // Or rather !initialized or something
        shader = new Shader("lambertian.glsl", GL_FRAGMENT_SHADER);
    auto [data, width, height] = readBMP(textureFile);
}


Shader* TextureMaterial::GetShader()
{
    return shader;
}


void TextureMaterial::SetUniforms(unsigned int program)
{
    GLuint kdLocation = glGetUniformLocation(program, "Kd");
    glUniform3f(kdLocation, Kd.x, Kd.y, Kd.z);
}


Shader* TextureMaterial::shader = nullptr;
