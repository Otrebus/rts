#include "TextureMaterial.h"
#include "Vector3.h"
#include "Shader.h"
#include "Utils.h"


TextureMaterial::TextureMaterial(const std::string& textureFile)
{
    if(!shader) // Or rather !initialized or something
        shader = new Shader("texturefragment.glsl", GL_FRAGMENT_SHADER);
    auto [data, width, height] = readBMP(textureFile);

    std::vector<unsigned char> data2;
    for(int i = 0; i < width*height; i++) {
        auto [r, g, b] = vectorToRgb(data[i]);
        data2.push_back(b);
        data2.push_back(g);
        data2.push_back(r);
    }

    glGenTextures(1, &this->texture); 

    //glGenVertexArrays(1, &VAO);
    //glBindVertexArray(VAO);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2.data());
}


Shader* TextureMaterial::GetShader()
{
    return shader;
}


void TextureMaterial::SetUniforms(unsigned int program)
{
}


Shader* TextureMaterial::shader = nullptr;
