#include "Tank.h"
#include "Model3d.h"
#include "Utils.h"

class Scene;
class Vector3;

Tank::Tank(Vector3 pos, Vector3 dir, Vector3 up, real width) : Entity(pos, dir, up), turnRate(0), velocity({ 0, 0 }), acceleration(0)
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


void Tank::accelerate(Vector2 accTarget)
{
    turn((this->dir%Vector3(accTarget.x, accTarget.y, 0.f)).z > 0);

    auto linearAcc = Vector2((dir.normalized()*acceleration).x, (dir.normalized()*acceleration).y);
    auto turnAcc = Vector2(-dir.y, dir.x)*turnRate*velocity.length();

    auto dir = Vector2(this->dir.x, this->dir.y);

    auto projAcc = turnAcc.length()/(turnAcc.normalized()*accTarget.normalized());
    acceleration = std::max(-maxAcc, std::min(maxAcc, (accTarget.normalized()*projAcc)*dir));

    auto acc = Vector2(-dir.y, dir.x)*turnRate*velocity.length() + linearAcc;

    auto ac = (Vector2(dir.x, dir.y).normalized()*(accTarget-acc));
    auto acceleration2 = std::max(-maxAcc, std::min(maxAcc, ac));
    
    if((dir.normalized()*acceleration2)*accTarget > (dir.normalized()*acceleration)*accTarget)
        acceleration = acceleration2;

    //std::cout << acceleration << std::endl;
}


void Tank::turn(bool left)
{
    turnRate = std::min(maxTurnRate, maxTurnAcc/velocity.length());
    if(!left)
        turnRate = -turnRate;
}

void Tank::updatePosition(real dt)
{
    pos += Vector3(velocity.x, velocity.y, 0)*dt;
    velocity = Vector2(dir.x, dir.y).normalized()*velocity.length();
    velocity += Vector2(dir.x, dir.y).normalized()*acceleration*dt;
    if(velocity.length() > maxSpeed)
        velocity = velocity.normalized()*maxSpeed;

    auto newDir = Vector2(dir.x, dir.y).normalized().rotated(turnRate*dt);
    dir = Vector3(newDir.x, newDir.y, 0).normalized();
    std::cout << dir << " " << velocity << std::endl;
}

Vector2 Tank::getVelocity() const
{
    return velocity;
}
