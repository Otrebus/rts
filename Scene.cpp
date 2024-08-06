#include "Entity.h"
#include "Scene.h"
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
        //std::cout << "entering " << std::setprecision(10) << entity->geoPos << std::endl;
        //std::cout << "geoVelocity is " << std::setprecision(10) << entity->geoVelocity << std::endl;
        auto pos2 = entity->geoPos + entity->geoVelocity*dt;

        // Collision detection, against the terrain
        auto [t, norm] = terrain->intersectCirclePathOcclusion(entity->geoPos, pos2, 0.51);
        auto [s, norm2] = terrain->intersectCirclePathOcclusion(entity->geoPos, pos2, 0.50);
        if(s > -inf && s <= 0)
        {
            // TODO: sometimes we find ourselves inside a triangle, what do we do then?
            //       if we blindly go into the if below we will just get stuck. For now
            //       we push ourselves slightly out of the triangle and hope this will
            //       resolve things eventually, but this isn't really robust
            //__debugbreak();
            entity->lastBumped = glfwGetTime();
            entity->geoPos += norm*0.01f;
            gliding.insert(entity);
            
            auto vn1 = (entity->geoVelocity*norm2)*norm2;
            auto vp1 = entity->geoVelocity - vn1 + norm2*0.1;

            entity->geoVelocity = vp1;
            //std::cout << "------------------------" << std::endl;
            //std::cout << "inside triangle, moved to " << entity->geoPos << std::endl;

            //__debugbreak();
            continue;
        }

        //t = std::max(t, t-0.001f);
        auto t2 = (pos2 - entity->geoPos).length();
        //std::cout << "t and t2 are " << t << " " << t2 << std::endl;
        if(!t)
        {
            entity->lastBumped = glfwGetTime();
            gliding.insert(entity);
        }
        if(t > 0 && t < t2 && entity->geoVelocity*norm < 0)
        {
            entity->lastBumped = glfwGetTime();
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
        if(!dynamic_cast<Unit*>(entity))
            continue;

        auto t = minT < inf ? minT : dt;
        auto preGeoPos = entity->geoPos;

        auto posNext = entity->geoPos + t*entity->geoVelocity;

        entity->geoPos = posNext;
        //std::cout << std::setprecision(10) << "posNext is " << posNext << std::endl;
        entity->plant(*terrain);
        //std::cout << std::setprecision(10) <<  "postPlant " << entity->geoPos << std::endl;
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
            //std::cout << "Position " << e1->geoPos << " moved towards " << e1->geoVelocity << std::endl;
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
        if(!dynamic_cast<Unit*>(entity))
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
            //std::cout << "not gliding" << std::endl;
            entity->geoVelocity = n*pV*sgn(n*entity->geoVelocity) + T*(entity->geoVelocity*T);
        }
        for(auto entity2 : entities)
        {
            if(entity2 == entity)
                continue;
            auto r = (entity2->geoPos-entity->geoPos);
            auto l = r.length();
            if(l < 1)
            {
                real modifier = std::max(0.0f, 1/(1.0f-entity->getVelocity().length()));
                entity2->geoVelocity += modifier*r*dt/l;
            }
        }
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
