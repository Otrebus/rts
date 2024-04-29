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
    this->units = units;
}

const std::vector<Unit*>& Scene::getUnits() const
{
    return units;
}

void Scene::setEntities(std::vector<Entity*> entities)
{
    this->entities = entities;
}

const std::vector<Entity*>& Scene::getEntities() const
{
    return entities;
}

void Scene::addEntity(Entity* entity)
{
    entities.push_back(entity);
}
