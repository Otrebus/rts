#include "TexturedTerrainMaterial.h"
#include "Vector3.h"
#include "Shader.h"
#include "Scene.h"
#include "Terrain.h"


TexturedTerrainMaterial::TexturedTerrainMaterial()
{     
    if(!fragmentShader)
        fragmentShader = new Shader("texturedTerrain.frag", GL_FRAGMENT_SHADER);

    auto [data2, width2, height2] = readBMP("Grass_01.bmp");
    auto [data3, width3, height3] = readBMP("splatmap.bmp");
    auto [data, width, height] = readBMP("Dirt_02.bmp");

    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());

    glGenTextures(1, &texture2);

    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, data2.data());

    glGenTextures(1, &splatMap);

    glBindTexture(GL_TEXTURE_2D, splatMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width3, height3, 0, GL_RGB, GL_UNSIGNED_BYTE, data3.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}


TexturedTerrainMaterial::~TexturedTerrainMaterial()
{
    glDeleteTextures(1, &texture);
    glDeleteTextures(1, &texture2);
    glDeleteTextures(1, &splatMap);
}


Shader* TexturedTerrainMaterial::getShader()
{
    return fragmentShader;
}


void TexturedTerrainMaterial::updateUniforms(Scene* scene)
{
    GLuint shaderProgramId = scene->getShaderProgram()->getId();
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(shaderProgramId, "texture1"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glUniform1i(glGetUniformLocation(shaderProgramId, "texture2"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, splatMap);
    glUniform1i(glGetUniformLocation(shaderProgramId, "texture3"), 2);

    int width = scene->getTerrain()->getWidth();
    int height = scene->getTerrain()->getHeight();

    glUniform3fv(glGetUniformLocation(shaderProgramId, "camPos"), 1, (GLfloat*)&scene->getCamera()->getPos());
    glUniform1i(glGetUniformLocation(shaderProgramId, "terrainWidth"), width);
    glUniform1i(glGetUniformLocation(shaderProgramId, "terrainHeight"), height);
}


Material* TexturedTerrainMaterial::clone()
{
    return new TexturedTerrainMaterial(*this);
}


Shader* TexturedTerrainMaterial::fragmentShader = nullptr;
