#include "Entity.h"
#include "LambertianMaterial.h"
#include "Model3d.h"
#include "TankWreck.h"
#include "FogOfWarMaterial.h"


TankWreck::TankWreck(Vector3 pos, Vector3 dir, Vector3 up, Terrain* terrain) : Entity(pos, dir, up)
{
    if(!fowMaterial)
        fowMaterial = new FogOfWarMaterial();

    body = ModelManager::instantiateModel("wreckbody");
    turret = ModelManager::instantiateModel("wreckturret");
    gun = ModelManager::instantiateModel("wreckbarrel");

    body->setScene(scene);
    turret->setScene(scene);
    gun->setScene(scene);

    turretDir = Vector3(1.f, 1.f, 0.f).normalized();
    gunPos = Vector3(0.f, 0.18f, 0.f);
    turretPos = Vector3(0.f, 0.f, 0.18f);

    geoPos = pos.to2();
    geoDir = dir.to2();
    geoVelocity = Vector2(0.f, 0.f);

    boundingBox = tankWreckBoundingBox;

    for(auto model : { body, turret, gun })
    {
        model->setPosition(pos);
        model->setDirection(dir, up);
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

    real length = 1;

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
            ((LambertianMaterial*)(mesh->getMaterial()))->Kd = { 0.1f, 0.1f, 0.1f };
        }
    }

    auto w = (bb.c2.x - bb.c1.x);
    auto d = (bb.c2.y - bb.c1.y);
    auto h = (bb.c2.z - bb.c1.z);
    auto ratio = length/w;

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
                v.pos *= length/w;
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
    auto width = d*ratio;
    tankWreckBoundingBox = BoundingBox(Vector3(-length/2, -width/2, -height/2), Vector3(length/2, width/2, height/2));

    body->init();
    gun->init();
    turret->init();
}

void TankWreck::setPosition(Vector3 pos)
{
    Vector3 delta = pos-this->pos;
    this->pos = pos;
    body->setPosition(pos);
    turret->setPosition(turret->getPosition()+delta);
    gun->setPosition(gun->getPosition()+delta);
}


void TankWreck::setDirection(Vector3 dir, Vector3 up)
{
    this->dir = dir;
    this->up = up;
    body->setDirection(dir, up);
}

bool TankWreck::canBeExtracted()
{
    return true;
}

void TankWreck::drawTurret(Material* mat)
{
    turret->setDirection(absTurDir, absTurUp);
    turret->setPosition(absTurPos);

    gun->setDirection(absGunDir, absGunUp);
    gun->setPosition(absGunPos);

    turret->draw(mat);
    gun->draw(mat);
}

void TankWreck::draw(Material* mat)
{
    GLint curDepthFun;
    GLboolean curBlend;
    GLint curSrc, curDst;

    glGetIntegerv(GL_DEPTH_FUNC, &curDepthFun);
    glGetBooleanv(GL_BLEND, &curBlend);
    glGetIntegerv(GL_BLEND_SRC_RGB, &curSrc);
    glGetIntegerv(GL_BLEND_DST_RGB, &curDst);
    
    glBlendFuncSeparate(GL_ZERO, GL_ONE, GL_ONE, GL_ZERO);

    body->draw(fowMaterial);
    drawTurret(fowMaterial);

    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);

    body->draw(mat);
    drawTurret(mat);

    if(!curBlend)
        glDisable(GL_BLEND);
    glDepthFunc(curDepthFun);
    glBlendFunc(curSrc, curDst);
}

void TankWreck::update(real dt)
{
    plant(*scene->getTerrain());
    absTurPos = pos + (dir*turretPos.y + (dir%up).normalized()*turretPos.x + up*turretPos.z);

    absTurUp = up;
    absTurDir = (dir*turretDir.y + (dir%up).normalized()*turretDir.x).normalized();
    absGunDir = dir*turretDir.y + up*turretDir.z + (dir%up).normalized()*turretDir.x;

    absGunPos = absTurPos + absTurDir*gunPos.y + (absTurDir%up).normalized()*gunPos.x + gunPos.z*absTurUp - absGunDir*gunRecoilPos;
    absGunUp = (absGunDir%(absTurDir%up)).normalized();
}

void TankWreck::updateUniforms()
{
    body->updateUniforms();
    turret->updateUniforms();
    gun->updateUniforms();
}

real TankWreck::gunLength = 1.0f;
BoundingBox TankWreck::tankWreckBoundingBox = BoundingBox();
Material* TankWreck::fowMaterial = nullptr;
