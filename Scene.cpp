#include "Entity.h"
#include "Scene.h"
#include "Unit.h"
#include <algorithm>
#include <thread>

class Camera;
class ShaderProgramManager;

Scene::Scene(Camera* camera, ShaderProgramManager* shaderProgramManager) : camera(camera), shaderProgramManager(shaderProgramManager), id(1)
{
}

Camera* Scene::getCamera() const
{
    return camera;
}

ShaderProgramManager* Scene::getShaderProgramManager() const
{
    return shaderProgramManager;
}

ShaderProgram* Scene::getShaderProgram() const
{
    return shaderProgram;
}

void Scene::setShaderProgram(ShaderProgram* sp)
{
    shaderProgram = sp;
}

void Scene::setTerrain(Terrain* terrain)
{
    this->terrain = terrain;
}

Terrain* Scene::getTerrain() const
{
    return terrain;
}

std::vector<Unit*> Scene::getUnits() const
{
    return units;
}

void Scene::setEntities(std::vector<Entity*> entities)
{
    this->entities = std::vector(entities.begin(), entities.end());
}

const std::vector<Entity*>& Scene::getEntities() const
{
    return entities;
}

void Scene::addEntity(Entity* entity)
{
    entities.push_back(entity);
    entity->setId(id);
    entityMap[id++] = entity;
    entity->scene = this;
}

void Scene::removeEntity(Entity* entity)
{
    deadEntities.insert(entity);
    entityMap.erase(entity->getId());
}

void Scene::updateEntities()
{
    std::lock_guard<std::mutex> guard(borrowMutex);
    std::vector<Entity*> newEntities;
    for(auto e : entities)
    {
        if(!deadEntities.contains(e))
            newEntities.push_back(e);
    }

    entities = newEntities;
    std::unordered_set<Entity*> newDeadEntities;
    for(auto& entity : deadEntities)
    {
        if(!borrowed[entity])
            delete entity;
        else
            newDeadEntities.insert(entity);
    }
    deadEntities = newDeadEntities;
}

void Scene::updateUnitList()
{
    units.clear();
    for(auto e : entities)
    {
        if(auto p = dynamic_cast<Unit*>(e); p)
            units.push_back(p);
    }
}

void Scene::borrow(Unit* unit)
{
    std::lock_guard<std::mutex> guard(borrowMutex);
    borrowed[unit]++;
}

void Scene::unBorrow(Unit* unit)
{
    std::lock_guard<std::mutex> guard(borrowMutex);
    borrowed[unit]--;
}

void Scene::addLight(PointLight* light)
{
    lights.push_back(light);
}

void Scene::removeLight(PointLight* light)
{
    deadLights.insert(light);
}

void Scene::updateLights()
{
    std::vector<PointLight*> newLights;
    for(auto e : lights)
        if(!deadLights.contains(e))
            newLights.push_back(e);
        else
            delete e;
    deadLights.clear();
    lights = newLights;
}

const std::vector<PointLight*>& Scene::getLights() const
{
    return lights;
}

void Scene::addParticle(Particle* particle)
{
    particles.push_back(particle);
}

const std::vector<Particle*>& Scene::getParticles() const
{
    return particles;
}

void Scene::updateParticles()
{
    std::vector<Particle*> newParticles;
    for(auto e : particles)
        if(e->isAlive())
            newParticles.push_back(e);
        else
            delete e;

    particles = newParticles;
}

Entity* Scene::getEntity(int id)
{
    if(entityMap.contains(id))
        return entityMap[id];
    return nullptr;
}