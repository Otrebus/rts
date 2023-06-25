#include "LambertianMaterial.h"
#include "Vector3.h"
#include "Shader.h"


LambertianMaterial::LambertianMaterial(Vector3 Kd)
{
    if(!shader) // Or rather !initialized or something
    {
        shader = new Shader("lambertian.glsl", GL_FRAGMENT_SHADER);
    }
}


Shader* LambertianMaterial::GetShader()
{
    return shader;
}


void LambertianMaterial::SetUniforms(unsigned int program)
{
    GLuint kdLocation = glGetUniformLocation(program, "Kd");
    glUniform3f(kdLocation, Kd.x, Kd.y, Kd.z);
}


Shader* LambertianMaterial::shader = nullptr;
