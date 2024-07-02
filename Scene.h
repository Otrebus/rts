#pragma once

#define NOMINMAX
#include "Camera.h"
#include "Particle.h"
#include "PointLight.h"
#include "ShaderProgram.h"
#include "ShaderProgramManager.h"
#include <memory>
#include <unordered_set>
#include <thread>
#include <mutex>

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
    const std::vector<Entity*>& getEntities() const;

    void addEntity(Entity* entity);
    void removeEntity(Entity* entity);
    void updateEntities();

    void addLight(PointLight* entity);
    void removeLight(PointLight* entity);
    const std::vector<PointLight*>& getLights() const;
    void updateLights();

    void addParticle(Particle* particle);
    void updateParticles();
    const std::vector<Particle*>& getParticles() const;

    void borrow(Unit* unit);
    void unBorrow(Unit* unit);

private:
    std::mutex borrowMutex;

    std::vector<Unit*> units;
    std::unordered_set<Unit*> deadUnits;

    std::vector<Entity*> entities;
    std::unordered_set<Entity*> deadEntities;

    std::vector<PointLight*> lights;
    std::unordered_set<PointLight*> deadLights;

    std::vector<Particle*> particles;

    std::unordered_map<Entity*, int> borrowed;

    Terrain* terrain;
    Camera* camera;
    ShaderProgramManager* shaderProgramManager;
    ShaderProgram* shaderProgram;
};
