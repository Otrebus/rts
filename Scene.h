#pragma once

#define NOMINMAX
#include "Camera.h"
#include "ShaderProgramManager.h"
#include "ShaderProgram.h"
#include <unordered_set>

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
    const std::unordered_set<Entity*>& getEntities() const;

    void addEntity(Entity* entity);
    void removeEntity(Entity* entity);
    void updateEntities();
private:
    std::vector<Unit*> units;
    
    std::unordered_set<Entity*> entities;
    std::unordered_set<Entity*> deadEntities;

    Terrain* terrain;
    Camera* camera;
    ShaderProgramManager* shaderProgramManager;
    ShaderProgram* shaderProgram;
};
