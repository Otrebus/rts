#include "Entity.h"
#include "Scene.h"
#include "Unit.h"
#include "Terrain.h"
#include "GeometryUtils.h"

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


void Scene::moveEntitiesSolid(real dt)
{
    if(dt < 0)
        return;

    real minT = inf;
    Collision minCol;

    for(auto entity: entities)
    {
        if(!dynamic_cast<Unit*>(entity))
            continue;
        auto pos2 = entity->geoPos + entity->geoVelocity*dt;

        // Collision detection, against the terrain
        auto [t, norm] = terrain->intersectCirclePathOcclusion(entity->geoPos, pos2, 0.5);
        if(!t)
        {
            // TODO: sometimes we find ourselves inside a triangle, what do we do then?
            //       if we blindly go into the if below we will just get stuck. For now
            //       we push ourselves slightly out of the triangle and hope this will
            //       resolve things eventually, but this isn't really robust
            entity->geoPos += norm*0.01f;
        }
        auto t2 = (pos2 - entity->geoPos).length();
        if(t > -inf && t < t2)
        {
            if(t < minT)
            {
                minT = t;
                minCol = Collision { entity, nullptr, t, norm };
            }
            //assert(t >= 0);

            //// A bit hacky, we do another pass to see if we hit anything perpendicularly to the normal as well
            //velocity2 = (entity->geoVelocity*norm.perp())*norm.perp();

            //auto pos2 = entity->geoPos + velocity2*dt;

            //auto [t, norm] = terrain->intersectCirclePathOcclusion(entity->geoPos, pos2, 0.5);
            //auto t2 = (pos2 - entity->geoPos).length();
            //if(t > -inf && t < t2 && entity->geoDir*norm < 0)
            //    velocity2 = { 0, 0 };
        }

        // Collision detection, against other units
        for(auto unit : entity->scene->getEntities())
        {
            if(unit != entity)
            {
                auto pos1 = entity->geoPos, pos2 = unit->geoPos;
                auto dir = entity->geoVelocity - unit->geoVelocity;
                if(dir)
                {
                    auto dirN = dir.normalized();
                    auto [t, norm] = intersectCircleCirclePath(pos2, 0.5, pos1, 0.5, dirN);
                    t = t/dir.length();
                    auto d = (pos2-pos1).length();
                    
                    if(t >= 0 && t < dt)
                    {
                        pos1 += (entity->geoPos-pos2).normalized()*(1-d)/2.f;
                        pos2 += (pos2-entity->geoPos).normalized()*(1-d)/2.f;

                        if(t < minT && t > -inf)
                        {
                            minT = t;
                            minCol = Collision { entity, unit, t, norm };
                        }
                    }
                    /*entity->geoPos = pos1;
                    unit->geoPos = pos2;*/
                }
            }
        }
    }

    //minT = dt; // tmp

    if(minT < inf)
    {
        if(minCol.entity2)
        {
            auto e1 = minCol.entity1, e2 = minCol.entity2;
            auto norm = minCol.normal;
            auto pos1 = e2->geoPos - norm - norm*1e-3;
            auto pos2 = e1->geoPos + norm + norm*1e-3;

            auto vn1 = (e1->geoVelocity*norm)*norm;
            auto vp1 = e1->geoVelocity - vn1;

            auto vn2 = (e2->geoVelocity*norm)*norm;
            auto vp2 = e2->geoVelocity - vn2;

            auto vn = (vn1-vn2)/2;

            e1->geoVelocity = vn;
            e2->geoVelocity = vn;

            e1->geoPos = pos1;
            e2->geoPos = pos2;
        }
        else
        {
        }
    }

    // TODO: for the colliding object(s), move them t-eps less so they don't actually intersect
    
    for(auto entity : entities)
    { 
        if(!dynamic_cast<Unit*>(entity))
            continue;

        auto t = minT < inf ? minT : dt;
        auto preGeoPos = entity->geoPos;
        auto posNext = entity->geoPos + entity->geoVelocity*(t-std::min(1e-6f, t));

        auto n = entity->geoDir.normalized().perp();
        auto T = entity->geoDir;

        auto perpV = n*(n*entity->geoVelocity);

        if(perpV)
        {
            auto pV = std::abs(perpV.length());
            pV = std::max(0.f, pV - t*2);

            entity->geoVelocity = perpV.normalized()*pV + entity->geoVelocity.length()*T;
        }
        
        entity->geoPos = posNext;
        entity->plant(*terrain);
    }

    if(minT < inf)
        moveEntities(dt-minT);
}

void Scene::moveEntitiesSoft(real dt, int depth)
{
    if(dt < 0)
        return;

    real minT = inf;
    Collision minCol;

    for(auto entity: entities)
    {
        if(!dynamic_cast<Unit*>(entity))
            continue;
        auto pos2 = entity->geoPos + entity->geoVelocity*dt;

        // Collision detection, against the terrain
        auto [t, norm] = terrain->intersectCirclePathOcclusion(entity->geoPos, pos2, 0.5);
        if(!t)
        {
            // TODO: sometimes we find ourselves inside a triangle, what do we do then?
            //       if we blindly go into the if below we will just get stuck. For now
            //       we push ourselves slightly out of the triangle and hope this will
            //       resolve things eventually, but this isn't really robust
            entity->geoPos += norm*0.01f;
            t = 0.01;
        }
        auto t2 = (pos2 - entity->geoPos).length();
        if(t > -inf && t < t2)
        {
            if(t < minT)
            {
                minT = t;
                minCol = Collision { entity, nullptr, t, norm };
            }
        }

        // Collision detection, against other units
        //for(auto unit : entity->scene->getEntities())
        //{
        //    if(unit != entity)
        //    {
        //        auto pos1 = entity->geoPos, pos2 = unit->geoPos;
        //        auto v = pos2 - pos1;
        //        auto d = v.length();
        //        pos1 += (entity->geoPos-pos2).normalized()*(1-d);
        //        pos2 += (pos2-entity->geoPos).normalized()*(1-d);
        //        if(d < 1)
        //            entity->geoPos -= dt*v.normalized()/(std::max(d, 0.3f));
        //    }
        //}
    }

    // TODO: for the colliding object(s), move them t-eps less so they don't actually intersect
    if(minT < inf)
    {
        if(minCol.entity1)
        {
            auto e1 = minCol.entity1;
            auto norm = minCol.normal;

            auto vn1 = (e1->geoVelocity*norm)*norm;
            auto vp1 = e1->geoVelocity - vn1;

            e1->geoPos += norm*0.01;
            e1->geoVelocity = vp1 + norm*0.00001;
            std::cout << "Position " << e1->geoPos << " moved towards " << e1->geoVelocity << std::endl;
        }
    }

    for(auto entity : entities)
    { 
        if(!dynamic_cast<Unit*>(entity))
            continue;

        auto t = minT < inf ? minT : dt;
        auto preGeoPos = entity->geoPos;
        auto posNext = entity->geoPos + entity->geoVelocity*(t-std::min(1e-6f, t));

        auto n = entity->geoDir.normalized().perp();
        auto T = entity->geoDir;

        auto perpV = n*(n*entity->geoVelocity);

        if(perpV)
        {
            auto pV = std::abs(perpV.length());
            pV = std::max(0.f, pV - t*2);

            entity->geoVelocity = n*pV + entity->geoVelocity.length()*T;
        }
        
        entity->geoPos = posNext;
        entity->plant(*terrain);
    }
    moveEntitiesSoft(dt-minT, depth+1);
}

void Scene::moveEntities(real dt)
{
    //moveEntitiesSolid(dt);
    moveEntitiesSoft(dt, 1);
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
