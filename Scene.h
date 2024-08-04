#pragma once

#include "Camera.h"
#include "Particle.h"
#include "PointLight.h"
#include "ShaderProgram.h"
#include "ShaderProgramManager.h"
#include <unordered_set>
#include <mutex>
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

    std::vector<Unit*> getUnits() const;

    void setEntities(std::vector<Entity*> entities);
    const std::vector<Entity*>& getEntities() const;

    Entity* getEntity(int id);
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

    void updateUnitList();
    void moveEntities(real dt);
    void moveEntitiesSoft(real dt, int depth, std::unordered_set<Entity*>& gliding);

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

    std::unordered_map<int, Entity*> entityMap;

    int id;

    Terrain* terrain;
    Camera* camera;
    ShaderProgramManager* shaderProgramManager;
    ShaderProgram* shaderProgram;
};
