#include "Entity.h"
#include "Scene.h"
#include "Rock.h"
#include "TankWreck.h"
#include "Unit.h"
#include "Terrain.h"
#include "GeometryUtils.h"
#include <iomanip>

#include "ShapeDrawer.h"

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

std::vector<Building*> Scene::getBuildings() const
{
    return buildings;
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

struct Collision
{
    Entity* entity1;
    Entity* entity2;

    real t;
    Vector2 normal;
};

void Scene::moveEntitiesSoft(real dt, int depth, std::unordered_set<Entity*>& gliding)
{
    if(dt < 0)
        return;

    real minT = inf;
    Collision minCol;

    for(auto entity: entities)
    {
        if(!dynamic_cast<Unit*>(entity))
            continue;
        if(!entity->geoVelocity)
            continue;
        auto pos2 = entity->geoPos + entity->geoVelocity*dt;

        // Collision detection, against the terrain
        auto [t, norm] = terrain->intersectCirclePathOcclusion(entity->geoPos, pos2, 0.51f);
        auto [s, norm2] = terrain->intersectCirclePathOcclusion(entity->geoPos, pos2, 0.50f);
        if(s > -inf && s <= 0)
        {
            // TODO: sometimes we find ourselves inside a triangle, what do we do then?
            //       if we blindly go into the if below we will just get stuck. For now
            //       we push ourselves slightly out of the triangle and hope this will
            //       resolve things eventually, but this isn't really robust
            //__debugbreak();
            entity->lastBumped = real(glfwGetTime());
            entity->geoPos += norm*0.01f;
            gliding.insert(entity);
            
            auto vn1 = (entity->geoVelocity*norm2)*norm2;
            auto vp1 = entity->geoVelocity - vn1 + norm2*0.1f;

            entity->geoVelocity = vp1;
            continue;
        }

        auto t2 = (pos2 - entity->geoPos).length();
        if(!t)
        {
            entity->lastBumped = real(glfwGetTime());
            gliding.insert(entity);
        }
        if(t > 0 && t < t2 && entity->geoVelocity*norm < 0)
        {
            entity->lastBumped = real(glfwGetTime());
            auto T = dt*(t/t2);
            gliding.insert(entity);
            if(T < minT && T < dt)
            {
                minT = T;
                minCol = Collision { entity, nullptr, T, norm };
            }
        }
    }

    for(auto entity : entities)
    { 
        if(!dynamic_cast<Unit*>(entity) || dynamic_cast<Building*>(entity) || dynamic_cast<Rock*>(entity) ) // TODO: bad pattern
            continue;

        auto t = minT < inf ? minT : dt;
        auto preGeoPos = entity->geoPos;
        auto prePos = entity->getPosition();

        auto posNext = entity->geoPos + t*entity->geoVelocity;

        entity->geoPos = posNext;
        entity->plant(*terrain);
        entity->setVelocity((entity->getPosition() - prePos)/dt);
    }

    if(minT < inf)
    {
        if(minCol.entity1)
        {
            auto e1 = minCol.entity1;
            auto norm = minCol.normal;

            auto vn1 = (e1->geoVelocity*norm)*norm;
            auto vp1 = e1->geoVelocity - vn1;

            e1->geoVelocity = vp1;
        }
    }

    moveEntitiesSoft(dt-minT, depth+1, gliding);
}

void Scene::moveEntities(real dt)
{
    std::unordered_set<Entity*> gliding;
    moveEntitiesSoft(dt, 1, gliding);

    for(auto entity : entities)
    { 
        if(!dynamic_cast<Unit*>(entity) || dynamic_cast<Building*>(entity)) // TODO: bad pattern
            continue;

        auto t = dt;
        auto preGeoPos = entity->geoPos;

        auto posNext = entity->geoPos + t*entity->geoVelocity;

        auto n = entity->geoDir.normalized().perp();
        auto T = entity->geoDir;

        auto perpV = n*(n*entity->geoVelocity);

        if(glfwGetTime() - entity->lastBumped > 0.3 && perpV)
        {
            auto pV = perpV.length();
            pV = std::max(0.f, pV - t*2);
            entity->geoVelocity = n*pV*sgn(n*entity->geoVelocity) + T*(entity->geoVelocity*T);
        }
        for(auto entity2 : entities)
        {
            if(entity2 == entity || dynamic_cast<Building*>(entity2))
                continue;
            auto r = (entity2->geoPos-entity->geoPos);
            auto l = r.length();
            if(l < 1)
            {
                real modifier = std::max(0.f, 1/(1.0f-entity->getVelocity().length()));
                if(!dynamic_cast<TankWreck*>(entity2) && !dynamic_cast<Building*>(entity2))
                    entity2->geoVelocity += modifier*r*dt/l;
            }
        }
        /*for(auto building : buildings)
        {
            auto bpos = building->getGeoPosition();
            auto c = Vector2(bpos.x + real(building->length)/2, bpos.y + real(building->width)/2);
            auto dx = entity->geoPos.x - c.x;
            auto dy = entity->geoPos.y - c.y;
            if(std::abs(dx)-0.5 < real(building->length)/2 && std::abs(dy)-0.5 < real(building->width)/2)
            {
                entity->geoVelocity += dt*Vector2(1/dx, 1/dy);
            }
        }*/
    }
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
    std::lock_guard<std::mutex> guard(buildingMutex);
    units.clear(); 
    buildings.clear(); // TODO: either this function should be renamed or this should be moved to another function

    for(auto e : entities)
    {
        if(auto p = dynamic_cast<Unit*>(e); p)
            units.push_back(p);
        if(auto p = dynamic_cast<Building*>(e); p)
            buildings.push_back(p);
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
