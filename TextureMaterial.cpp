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

    program = new ShaderProgram();

    program->AddShaders(*vertexShader, *fragmentShader);
    program->Use();
    auto [data, width, height] = readBMP(textureFile);

    glGenTextures(1, &this->texture); 

    //glGenVertexArrays(1, &VAO);
    //glBindVertexArray(VAO);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
}


void TextureMaterial::Use()
{
    glBindTexture(GL_TEXTURE_2D, texture);
    program->Use();
}


void TextureMaterial::UpdateUniforms(Scene* scene)
{
    program->Use();
    glUniform3fv(glGetUniformLocation(program->GetId(), "camPos"), 1, (GLfloat*) &scene->GetCamera()->pos);
    glUniformMatrix4fv(glGetUniformLocation(program->GetId(), "transform"), 1, GL_TRUE, (float*)&scene->GetCamera()->GetMatrix(59, 16./10.).m_val);
}


Shader* TextureMaterial::fragmentShader = nullptr;
Shader* TextureMaterial::vertexShader = nullptr;
