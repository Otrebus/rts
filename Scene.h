#pragma once

#define NOMINMAX
#include "Camera.h"
#include "ShaderProgramManager.h"
#include "ShaderProgram.h"

class Unit;


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

    void setUnits(std::vector<Unit*> units);
    const std::vector<Unit*>& getUnits() const;

    void setEntities(std::vector<Entity*> entities);
    const std::vector<Entity*>& getEntities() const;

    void addEntity(Entity* entity);
private:
    std::vector<Unit*> units;
    std::vector<Entity*> entities;
    Terrain* terrain;
    Camera* camera;
    ShaderProgramManager* shaderProgramManager;
    ShaderProgram* shaderProgram;
};
