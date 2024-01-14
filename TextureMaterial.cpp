#include "TextureMaterial.h"
#include "Vector3.h"
#include "Shader.h"
#include "Utils.h"


TextureMaterial::TextureMaterial(const std::string& textureFile)
{
    if(!fragmentShader) // Or rather !initialized or something
        fragmentShader = new Shader("texture.frag", GL_FRAGMENT_SHADER);
    if(!vertexShader)
        vertexShader = new Shader("vertexShader.vert", GL_VERTEX_SHADER);
    if(!geometryShader)
        geometryShader = new Shader("geometryShader.geom", GL_GEOMETRY_SHADER);

    program = new ShaderProgram();

    program->addShaders(*vertexShader, *fragmentShader, *geometryShader);
    program->use();
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
}


void TextureMaterial::use()
{
    glBindTexture(GL_TEXTURE_2D, texture);
    program->use();
}


void TextureMaterial::updateUniforms(Scene* scene)
{
    program->use();
    glUniform3fv(glGetUniformLocation(program->getId(), "camPos"), 1, (GLfloat*) &scene->getCamera()->pos);
    glUniformMatrix4fv(glGetUniformLocation(program->getId(), "transform"), 1, GL_TRUE, (float*)&scene->getCamera()->getMatrix().m_val);
}


Shader* TextureMaterial::fragmentShader = nullptr;
Shader* TextureMaterial::vertexShader = nullptr;
Shader* TextureMaterial::geometryShader = nullptr;