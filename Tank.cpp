#include "Tank.h"
#include "Model3d.h"

class Scene;
class Vector3;

Tank::Tank(Vector3 pos, Vector3 dir, Vector3 up) : Entity(pos, dir, up)
{
    body = new Model3d("tankbody.obj");
    turret = new Model3d("tankturret.obj");
    gun = new Model3d("tankbarrel.obj");
}


Tank::~Tank()
{
}


void Tank::setUp(Scene* scene)
{
    body->setUp(scene);
    turret->setUp(scene);
    gun->setUp(scene);
}


void Tank::updateUniforms()
{
    body->updateUniforms();
    turret->updateUniforms();
    gun->updateUniforms();
}


void Tank::draw()
{
    body->draw();
    turret->draw();
    gun->draw();
}