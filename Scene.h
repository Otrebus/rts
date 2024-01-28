#pragma once

#include "Camera.h"
#include "ShaderProgramManager.h"
#include "ShaderProgram.h"

class Scene
{
public:
    Scene(Camera* camera, ShaderProgramManager* shaderProgramManager);
    Camera* getCamera() const;
    ShaderProgramManager* getShaderProgramManager() const;
    void setShaderProgram(ShaderProgram* program);
    ShaderProgram* getShaderProgram() const;

private:
    Camera* camera;
    ShaderProgramManager* shaderProgramManager;
    ShaderProgram* shaderProgram;
};
