#include "Shader.h"
#include "TextureMaterial.h"
#include "Utils.h"
#include "Vector3.h"


TextureMaterial::TextureMaterial(const std::string& textureFile)
{
    if(!fragmentShader) // Or rather !initialized or something
        fragmentShader = new Shader("texture.frag", GL_FRAGMENT_SHADER);

    auto [data, width, height] = readBMP(textureFile);

    glGenTextures(1, &this->texture);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
}


TextureMaterial::~TextureMaterial()
{
    glDeleteTextures(1, &texture);
}


Shader* TextureMaterial::getShader()
{
    return fragmentShader;
}


void TextureMaterial::updateUniforms(Scene* scene)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform3fv(glGetUniformLocation(scene->getShaderProgram()->getId(), "camPos"), 1, (GLfloat*)&scene->getCamera()->getPos());
}


Material* TextureMaterial::clone()
{
    return new TextureMaterial(*this);
}


Shader* TextureMaterial::fragmentShader = nullptr;
