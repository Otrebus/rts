#include "Tank.h"
#include "Model3d.h"
#include "Utils.h"

class Scene;
class Vector3;

Tank::Tank(Vector3 pos, Vector3 dir, Vector3 up, real width) : Entity(pos, dir, up)
{
    body = new Model3d("tankbody.obj");
    turret = new Model3d("tankturret.obj");
    gun = new Model3d("tankbarrel.obj");

    BoundingBox bb;

    for(auto model : { body, turret, gun }) {
        for(auto& mesh : model->getMeshes()) {
            for(auto& v : ((Mesh3d*) mesh)->v) {
                v.pos = Vector3(v.pos.z, v.pos.x, v.pos.y);
                v.normal = Vector3(v.normal.z, v.normal.x, v.normal.y);
                bb.c1.x = std::min(bb.c1.x, v.pos.x);
                bb.c1.y = std::min(bb.c1.y, v.pos.y);
                bb.c1.z = std::min(bb.c1.z, v.pos.z);
                bb.c2.x = std::max(bb.c2.x, v.pos.x);
                bb.c2.y = std::max(bb.c2.y, v.pos.y);
                bb.c2.z = std::max(bb.c2.z, v.pos.z);
            }
        }
    }

    auto w = (bb.c2.x - bb.c1.x);
    auto h = (bb.c2.y - bb.c1.y);
    auto d = (bb.c2.z - bb.c1.z);

    for(auto model : { body, turret, gun }) {
        for(auto& mesh : model->getMeshes()) {
            for(auto& v : ((Mesh3d*) mesh)->v) {
                v.pos -= Vector3((bb.c2.x + bb.c1.x)/2, (bb.c2.y + bb.c1.y)/2, bb.c1.z);
                v.pos *= width/w;
            }
        }
        model->setPosition(pos);
        model->setDirection(dir, up);
    }
}


Tank::~Tank()
{
}


void Tank::setUp(Scene* scene)
{
    body->setUp(scene);
    turret->setUp(scene);
    gun->setUp(scene);
    boundingBoxModel->setUp(scene);
}


void Tank::updateUniforms()
{
    body->updateUniforms();
    turret->updateUniforms();
    gun->updateUniforms();
    boundingBoxModel->updateUniforms();
}


void Tank::draw()
{
    body->draw();
    turret->draw();
    gun->draw();
}


void Tank::setPosition(Vector3 pos)
{
    this->pos = pos;
    body->setPosition(pos);
    turret->setPosition(pos);
    gun->setPosition(pos);
    boundingBoxModel->setPosition(pos);
}


void Tank::setDirection(Vector3 dir, Vector3 up)
{
    this->dir = dir;
    this->up = up;
    body->setDirection(dir, up);
    turret->setDirection(dir, up);
    gun->setDirection(dir, up);
    boundingBoxModel->setDirection(dir, up);
}


void Tank::accelerate(Vector3 dir)
{
}