#include "Tank.h"
#include "Model3d.h"
#include "Utils.h"
#include "BoundingBoxModel.h"
#include "Terrain.h"

class Scene;
class Vector3;

// TODO: no need to get terrain since we have scene->getTerrain()
Tank::Tank(Vector3 pos, Vector3 dir, Vector3 up, real width, Terrain* terrain) : Entity(pos, dir, up), turnRate(0), acceleration(0), terrain(terrain)
{
    body = new Model3d("tankbody.obj");
    turret = new Model3d("tankturret.obj");
    gun = new Model3d("tankbarrel.obj");

    geoPos = pos.to2();
    geoDir = dir.to2();

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
    auto d = (bb.c2.y - bb.c1.y);
    auto h = (bb.c2.z - bb.c1.z);

    auto ratio = width/w;

    for(auto model : { body, turret, gun }) {
        for(auto& mesh : model->getMeshes()) {
            for(auto& v : ((Mesh3d*) mesh)->v) {
                v.pos -= Vector3((bb.c2.x + bb.c1.x)/2, (bb.c2.y + bb.c1.y)/2, (bb.c2.z-bb.c1.z)/2);
                v.pos *= width/w;
            }
        }
        model->setPosition(pos);
        model->setDirection(dir, up);
    }

    height = h*ratio;
    depth = d*ratio;
    boundingBoxModel = new BoundingBoxModel(pos, dir, up, width, depth, height);
    boundingBox = BoundingBox(Vector3(-width/2, -depth/2, -height/2), Vector3(width/2, depth/2, height/2));
}


Tank::~Tank()
{
}


void Tank::setUp(Scene* scene)
{
    this->scene = scene;
    body->setUp(scene);
    turret->setUp(scene);
    gun->setUp(scene);
    boundingBoxModel->setUp(scene);
    destinationLine.setUp(scene);
    destinationLine.setColor(Vector3(0, 1, 0));
    destinationLine.setInFront(true);
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
    //destinationLine.setVertices( { pos, target });
    std::vector<Vector3> P;
    // TODO: slow
    for(auto p : path)
        P.push_back( { p.x, p.y, terrain->getElevation(p.x, p.y) });
    P.push_back(getPosition());
    destinationLine.setVertices(P);

    body->draw();
    turret->draw();
    gun->draw();
    if(selected)
        boundingBoxModel->draw();

    if(selected && target.length() > 0)
    {
        destinationLine.draw();
    }
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


void Tank::accelerate(Vector2 velocityTarget)
{
    auto accTarget = velocityTarget - velocity;
    if(!accTarget)
    {
        turnRate = 0;
        acceleration = 0;
        return;
    }

    if(velocityTarget.length() > 0.01 && velocityTarget.normalized()*geoDir < 0.999)
    {
        std::cout << velocityTarget.normalized() << " " << geoDir << " " << velocityTarget.normalized()*geoDir << std::endl;
        turn(geoDir%velocityTarget > 0);
    }
    else
        turnRate = 0;

    auto turnAcc = Vector2(-dir.y, dir.x)*turnRate*maxSpeed;

    Line3d line({
        pos,
        pos + turnAcc.to3()
    });
    line.setUp(scene);
    line.setInFront(true);
    line.draw();

    auto projAcc = !turnAcc ? std::abs(accTarget*geoDir.normalized()) : turnAcc.length()/(turnAcc.normalized()*accTarget.normalized());

    acceleration = std::max(-maxBreakAcc, std::min(maxForwardAcc, (accTarget.normalized()*projAcc)*geoDir));
}

void Tank::brake()
{
    turnRate = 0;
    acceleration = -maxBreakAcc;
}

void Tank::turn(bool left)
{
    turnRate = std::min(maxTurnRate, maxTurnAcc/velocity.length());
    if(!left)
        turnRate = -turnRate;
}

void Tank::update(real dt)
{
    auto pos2 = geoPos + velocity*dt;

    // Collision detection, against the terrain
    auto [t, norm] = terrain->intersectCirclePathOcclusion(geoPos, pos2, 0.5);
    auto t2 = (pos2 - geoPos).length();
    if(t > -inf && t < t2 && geoDir*norm < 0)
    {
        assert(t >= 0);
        auto v2 = (pos2-geoPos) - norm*((pos2-geoPos)*norm);

        // TODO: this can cause the tank to move through another triangle, so
        //       what if we set the velocity in this direction instead?
        geoPos += v2;
    }
    else
        geoPos = pos2;

    // Collision detection, against other units
    for(auto entity : scene->getEntities())
    {
        if(entity != this)
        {
            auto pos1 = geoPos, pos2 = entity->geoPos;
            if(auto d = (pos1 - pos2).length(); d < 1)
            {
                pos1 += (geoPos-pos2).normalized()*(1-d)/2.f;
                pos2 += (pos2-geoPos).normalized()*(1-d)/2.f;
            }   
            geoPos = pos1;
            entity->geoPos = pos2;
        }
    }

    auto velocityTarget = boidCalc();

    accelerate(velocityTarget);

    auto newDir = geoDir.normalized().rotated(turnRate*dt);
    if((newDir%velocityTarget)*(geoDir%velocityTarget) < 0 && newDir*velocityTarget > 0)
        newDir = velocityTarget.normalized();
    geoDir = newDir;

    velocity = Vector2(geoDir.x, geoDir.y).normalized()*velocity.length();
    auto velocity1 = velocity;
    velocity += Vector2(geoDir.x, geoDir.y).normalized()*acceleration*dt;
    if(velocity1*velocity < 0 && !velocityTarget)
    {
        velocity = { 0, 0 };
        acceleration = 0;
    }

    if(velocity.length() > maxSpeed)
        velocity = velocity.normalized()*maxSpeed;
}

Vector2 Tank::seek()
{
    if(!path.empty())
    {
        auto target = path.back();

        if(target.length() > 0.0001) {
            auto l = (target - geoPos).length();
            if(l < 0.5)
                path.pop_back();

            // TODO: this could become NaN
            if(!l)
                return { 0, 0 };
            auto v2 = (target - geoPos).normalized();

            auto speed = maxSpeed;
            if(path.size() == 1)
            {
                if((target - geoPos).length() < 0.5)
                    return { 0, 0 };
                speed = std::min(maxSpeed, (target - geoPos).length());
            }
            return v2*speed;
        }
    }
    else
        return { 0, 0 };
}

Vector2 Tank::evade()
{
    for(auto entity : scene->getEntities())
    {
        if(entity != this)
        {
            auto pos1 = geoPos, pos2 = entity->geoPos;
            auto e = (pos2 - pos1);
            auto v1 = velocity, v2 = entity->getVelocity();
            auto v = v1 - v2;
            auto w = ((e*v)/v.length2())*v;

            auto r = w - (pos2-pos1);
            // TODO: better deduction of time-to-collision (e/v)?
            if(e*v > 0 && r.length() < 1 && e.length()/v.length() < 1) {
                std::cout << "evading" << std::endl;
                return v1%r > 1 ? std::min(w.length(), 3.f)*v1.perp() : -std::min(w.length(), 3.f)*v1.perp();
            }
        }
    }
    return { 0, 0 };
}

Vector2 Tank::boidCalc()
{
    return evade() + seek();
}
