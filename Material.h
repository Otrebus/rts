#pragma once

class Material
{
public:
    Material();

    virtual void SetUniforms(unsigned int program) = 0;
    Shader* GetShader();
};
