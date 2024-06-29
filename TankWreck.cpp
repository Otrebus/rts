#include "Entity.h"
#include "BoundingBoxModel.h"
#include "Entity.h"
#include "GeometryUtils.h"
#include "Math.h"
#include "LambertianMaterial.h"
#include "Model3d.h"
#include "Particle.h"
#include "PathFinding.h"
#include "Polysolver.h"
#include "Projectile.h"
#include "SelectionDecalMaterial.h"
#include "SelectionMarkerMesh.h"
#include "Tank.h"
#include "Terrain.h"
#include "Utils.h"
#include "TankWreck.h"


TankWreck::TankWreck(Vector3 pos, Vector3 dir, Vector3 up, Terrain* terrain) : Entity(pos, dir, up, width, height, depth)
{
    body = ModelManager::instantiateModel("tankbody");
    turret = ModelManager::instantiateModel("tankturret");
    gun = ModelManager::instantiateModel("tankbarrel");

    body->setScene(scene);
    turret->setScene(scene);
    gun->setScene(scene);

    turretDir = Vector3(1, 1, 0).normalized();
    gunPos = Vector3(0, 0.18, 0);
    turretPos = Vector3(0, 0, 0.18);

    geoPos = pos.to2();
    geoDir = dir.to2();

    boundingBox = tankWreckBoundingBox;

    for(auto model : { body, turret, gun })
    {
        model->setPosition(pos);
        model->setDirection(dir, up);
        //model->init(); // TODO: we're updating the VAOs etc, but not deleting the old ones
    }
}

void TankWreck::init(Scene* scene)
{
    this->scene = scene;
    body->setScene(scene);
    turret->setScene(scene);
    gun->setScene(scene);
}

void TankWreck::loadModels()
{
    auto body = new Model3d();
    body->readFromFile("tankbody.obj");

    auto turret = new Model3d();
    turret->readFromFile("tankturret.obj");

    auto gun = new Model3d();
    gun->readFromFile("tankbarrel.obj");

    ModelManager::addModel("wreckbody", body);
    ModelManager::addModel("wreckturret", turret);
    ModelManager::addModel("wreckbarrel", gun);

    real width = 1;

    BoundingBox bb;

    for(auto model : { body, turret, gun })
    {
        for(auto& mesh : model->getMeshes())
        {
            for(auto& v : ((Mesh3d*)mesh)->v)
            {
                v.pos = Vector3(v.pos.z, v.pos.x, v.pos.y);
                v.normal = Vector3(v.normal.z, v.normal.x, v.normal.y);
                bb.c1.x = std::min(bb.c1.x, v.pos.x);
                bb.c1.y = std::min(bb.c1.y, v.pos.y);
                bb.c1.z = std::min(bb.c1.z, v.pos.z);
                bb.c2.x = std::max(bb.c2.x, v.pos.x);
                bb.c2.y = std::max(bb.c2.y, v.pos.y);
                bb.c2.z = std::max(bb.c2.z, v.pos.z);
            }
            ((LambertianMaterial*)(mesh->getMaterial()))->Kd = { 0.1, 0.1, 0.1 };
        }
    }

    auto w = (bb.c2.x - bb.c1.x);
    auto d = (bb.c2.y - bb.c1.y);
    auto h = (bb.c2.z - bb.c1.z);
    auto ratio = width/w;

    for(auto model : { body, turret, gun })
    {
        BoundingBox bb;
        for(auto& mesh : model->getMeshes())
        {
            for(auto& v : ((Mesh3d*)mesh)->v)
            {
                bb.c1.x = std::min(bb.c1.x, v.pos.x);
                bb.c1.y = std::min(bb.c1.y, v.pos.y);
                bb.c1.z = std::min(bb.c1.z, v.pos.z);
                bb.c2.x = std::max(bb.c2.x, v.pos.x);
                bb.c2.y = std::max(bb.c2.y, v.pos.y);
                bb.c2.z = std::max(bb.c2.z, v.pos.z);
            }
        }
        for(auto& mesh : model->getMeshes())
        {
            for(auto& v : ((Mesh3d*)mesh)->v)
            {
                v.pos -= Vector3((bb.c2.x + bb.c1.x)/2, (bb.c2.y + bb.c1.y)/2, (bb.c2.z + bb.c1.z)/2);
                v.pos *= width/w;
            }
        }
    }
    real gunMinX = inf;
    real gunMaxX = -inf;
    for(auto& mesh : gun->getMeshes())
    {
        for(auto& v : ((Mesh3d*)mesh)->v)
        {
            gunMinX = std::min(v.pos.x, gunMinX);
            gunMaxX = std::max(v.pos.x, gunMaxX);
        }
    }
    for(auto& mesh : gun->getMeshes())
    {
        for(auto& v : ((Mesh3d*)mesh)->v)
            v.pos.x -= gunMinX;
    }
    gunLength = gunMaxX - gunMinX;

    auto height = h*ratio;
    auto depth = d*ratio;
    tankWreckBoundingBox = BoundingBox(Vector3(-width/2, -depth/2, -height/2), Vector3(width/2, depth/2, height/2));

    body->init();
    gun->init();
    turret->init();
}


void TankWreck::drawTurret()
{
    turret->setDirection(absTurDir, absTurUp);
    turret->setPosition(absTurPos);

    gun->setDirection(absGunDir, absGunUp);
    gun->setPosition(absGunPos);

    turret->draw();
    gun->draw();
}


void TankWreck::draw()
{
    body->draw();
    drawTurret();
}


void TankWreck::update(real dt)
{
}


void TankWreck::updateUniforms()
{
    body->updateUniforms();
    turret->updateUniforms();
    gun->updateUniforms();
}



real TankWreck::gunLength = 1.0f;
Model3d* TankWreck::tankWreckBoundingBoxModel = nullptr;
BoundingBox TankWreck::tankWreckBoundingBox = BoundingBox();