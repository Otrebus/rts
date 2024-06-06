#include "TexturedTerrainMaterial.h"
#include "Vector3.h"
#include "Shader.h"
#include "Scene.h"


TexturedTerrainMaterial::TexturedTerrainMaterial()
{     
    if(!fragmentShader)
        fragmentShader = new Shader("texturedTerrain.frag", GL_FRAGMENT_SHADER);

    auto [data, width, height] = readBMP("Grass_02.bmp");

    glGenTextures(1, &this->texture);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
}


TexturedTerrainMaterial::~TexturedTerrainMaterial()
{
}


Shader* TexturedTerrainMaterial::getShader()
{
    return fragmentShader;
}


void TexturedTerrainMaterial::updateUniforms(Scene* scene)
{
    glUniform3fv(glGetUniformLocation(scene->getShaderProgram()->getId(), "camPos"), 1, (GLfloat*) &scene->getCamera()->getPos());
}


void TexturedTerrainMaterial::setFlat(bool flat)
{
    isFlat = flat;
}

Shader* TexturedTerrainMaterial::fragmentShader = nullptr;
