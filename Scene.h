#pragma once

#define NOMINMAX
#include "Camera.h"
#include "ShaderProgramManager.h"
#include "ShaderProgram.h"
#include <unordered_set>
#include <memory>
#include "PointLight.h"

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

    void addUnit(Unit* unit);
    void setUnits(std::vector<Unit*> units);
    std::vector<Unit*> getUnits() const;
    void removeUnit(Unit* unit);
    void clearUnits();

    void setEntities(std::vector<Entity*> entities);
    const std::unordered_set<Entity*>& getEntities() const;

    void addEntity(Entity* entity);
    void removeEntity(Entity* entity);
    void updateEntities();

    void addLight(PointLight* entity);
    void removeLight(PointLight* entity);
    const std::unordered_set<PointLight*>& getLights() const;
    void updateLights();
private:
    std::vector<std::shared_ptr<Unit>> units;
    std::unordered_set<std::shared_ptr<Unit>> deadUnits;
    
    std::unordered_set<Entity*> entities;
    std::unordered_set<Entity*> deadEntities;

    std::unordered_set<PointLight*> lights;
    std::unordered_set<PointLight*> deadLights;

    Terrain* terrain;
    Camera* camera;
    ShaderProgramManager* shaderProgramManager;
    ShaderProgram* shaderProgram;
};
