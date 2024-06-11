#include "Scene.h"
#include "Entity.h"
#include "Unit.h"

class Camera;
class ShaderProgramManager;

Scene::Scene(Camera* camera, ShaderProgramManager* shaderProgramManager) : camera(camera), shaderProgramManager(shaderProgramManager)
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

void Scene::setUnits(std::vector<Unit*> units)
{
    std::vector<std::shared_ptr<Unit>> ptrs;
    for(auto unit : units)
        ptrs.push_back(std::shared_ptr<Unit>(unit));
    this->units = ptrs;
}

void Scene::addUnit(Unit* unit)
{
    this->units.push_back(std::shared_ptr<Unit>(unit));
}

std::vector<Unit*> Scene::getUnits() const
{
    std::vector<Unit*> U;
    for(auto& u : units)
        U.push_back(u.get());
    return U;
}

void Scene::removeUnit(Unit* unit)
{
    unit->setDead();
    deadUnits.insert(unit->shared_from_this());
}

void Scene::clearUnits()
{
    std::vector<std::shared_ptr<Unit>> newUnits;
    for(auto unit : units)
    {
        if(!deadUnits.contains(unit))
            newUnits.push_back(unit->shared_from_this());
    }
    deadUnits.clear();
    units = newUnits;
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
}

void Scene::removeEntity(Entity* entity)
{
    deadEntities.insert(entity);
}

void Scene::updateEntities()
{
    std::vector<Entity*> newEntities;
    for(auto e : entities)
        if(!deadEntities.contains(e))
            newEntities.push_back(e);

    entities = newEntities;
    for(auto& entity : deadEntities)
        delete entity;

    deadEntities.clear();
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
