#pragma once

#define NOMINMAX
#include "Camera.h"
#include "ShaderProgramManager.h"
#include "ShaderProgram.h"

class Entity;

class Scene
{
public:
    Scene(Camera* camera, ShaderProgramManager* shaderProgramManager);
    Camera* getCamera() const;
    ShaderProgramManager* getShaderProgramManager() const;
    void setShaderProgram(ShaderProgram* program);
    ShaderProgram* getShaderProgram() const;
    void setTerrain(Terrain* terrain);
    Terrain* getTerrain() const;

    void setEntities(std::vector<Entity*> entities);
    const std::vector<Entity*>& getEntities() const;

private:
    std::vector<Entity*> entities;
    Terrain* terrain;
    Camera* camera;
    ShaderProgramManager* shaderProgramManager;
    ShaderProgram* shaderProgram;
};
