#include "Tank.h"
#include "Model3d.h"
#include "Utils.h"
#include "BoundingBoxModel.h"
#include "Terrain.h"
#include "PathFinding.h"
#include "Entity.h"

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


    pathCalculationInterval = (500 + (rand() % 500))/1000.0f;
}


Tank::~Tank()
{
}


void Tank::init(Scene* scene)
{
    this->scene = scene;
    body->init(scene);
    turret->init(scene);
    gun->init(scene);
    boundingBoxModel->init(scene);
    destinationLine.init(scene);
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

    if(selected && path.size() > 0)
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
    accelerationTarget = velocityTarget - velocity;
    if(accelerationTarget < 1e-6)
    {
        turnRate = 0;
        acceleration = 0;
        return;
    }

    if(velocityTarget.length() > 0.01 && velocityTarget.normalized()*geoDir < 0.999)
        turn(geoDir%velocityTarget > 0);
    else {
        turnRate = 0;
    }

    auto radialAcc = Vector2(-dir.y, dir.x)*turnRate*maxSpeed;

    //Line3d line({
    //    pos,
    //    pos + turnAcc.to3()
    //});
    //line.setUp(scene);
    //line.setInFront(true);
    //line.draw();

    auto projAcc = !radialAcc ? std::abs(accelerationTarget*geoDir.normalized()) : radialAcc.length()/(radialAcc.normalized()*accelerationTarget.normalized());

    acceleration = std::max(-maxBreakAcc, std::min(maxForwardAcc, (accelerationTarget.normalized()*projAcc)*geoDir));
}

void Tank::brake()
{
    turnRate = 0;
    acceleration = -maxBreakAcc;
}

void Tank::turn(bool left)
{
    turnRate = std::min(maxTurnRate, maxRadialAcc/velocity.length());
    if(!left)
        turnRate = -turnRate;
}

void Tank::update(real dt)
{
    auto pos2 = geoPos + velocity*dt;

    Vector2 velocity2 = velocity;
    bool hitWall = false;

    // Collision detection, against the terrain
    auto [t, norm] = terrain->intersectCirclePathOcclusion(geoPos, pos2, 0.5);
    if(!t)
    {
        // TODO: sometimes we find ourselves inside a triangle, what do we do then?
        //       if we blindly go into the if below we will just get stuck. For now
        //       we push ourselves slightly out of the triangle and hope this will
        //       resolve things eventually, but this isn't really robust
        geoPos += norm*0.01f;
    }
    auto t2 = (pos2 - geoPos).length();
    if(t > -inf && t < t2 && geoDir*norm < 0)
    {
        assert(t >= 0);
        
        // A bit hacky, we do another pass to see if we hit anything perpendicularly to the normal as well
        velocity2 = (velocity*norm.perp())*norm.perp();

        auto pos2 = geoPos + velocity2*dt;

        auto [t, norm] = terrain->intersectCirclePathOcclusion(geoPos, pos2, 0.5);
        auto t2 = (pos2 - geoPos).length();
        if(t > -inf && t < t2 && geoDir*norm < 0)
            velocity2 = { 0, 0 };
    }
    
    // TODO: we should still close the distance here
    auto posNext = geoPos + velocity2*dt;

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

    geoPos = posNext;

    velocityTarget = boidCalc();

    accelerate(velocityTarget);

    auto newDir = geoDir.normalized().rotated(turnRate*dt);
    if((newDir%velocityTarget)*(geoDir%velocityTarget) < 0 && newDir*velocityTarget > 0)
        newDir = velocityTarget.normalized();
    geoDir = newDir;

    if(!hitWall)
    {
        velocity = Vector2(geoDir.x, geoDir.y).normalized()*velocity.length();
        auto velocity1 = velocity;
        velocity += Vector2(geoDir.x, geoDir.y).normalized()*acceleration*dt;
        if(velocity1*velocity < 0 && !velocityTarget)
        {
            velocity = { 0, 0 };
            acceleration = 0;
        }
    }

    if(velocity.length() > maxSpeed)
        velocity = velocity.normalized()*maxSpeed;

    if(!pathFindingRequest && glfwGetTime() - pathLastCalculated > pathCalculationInterval && path.size())
    {
        PathFindingRequest* request = new PathFindingRequest;
        request->requester = this;
        request->start = getPosition().to2();
        request->dest = path.front();
        setCurrentPathfindingRequest(request);
        addPathFindingRequest(request);
    }

}

Vector2 Tank::seek()
{
    if(!path.empty())
    {
        auto target = path.back();

        if(target.length() > 0.0001) {
            auto l = (target - geoPos).length();
            auto R = getArrivalRadius(target, scene->getEntities());

            if(l < R)
                path.pop_back();
            if(path.empty())
                return { 0, 0 };
            target = path.back();

            // TODO: this could become NaN
            if(!l) {
                return { 0, 0 };
            }
            auto v2 = (target - geoPos).normalized();

            auto speed = maxSpeed;
            if(path.size() == 1)
            {
                if((target - geoPos).length() < 0.5) {
                    return { 0, 0 };
                }
                speed = std::min(maxSpeed, (target - geoPos).length());
            }
            return v2*speed;
        }
    }
    else {
        return { 0, 0 };
    }
}

Vector2 Tank::evade()
{
    Vector2 sum = { 0, 0 };
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
                sum += (v1%r > 1 ? std::min(w.length(), 3.f)*v1.perp() : -std::min(w.length(), 3.f)*v1.perp());
            }
        }
    }
    return sum;
}


Vector2 Tank::avoid()
{
    auto pos2 = geoPos + velocity;

    auto [t, norm] = terrain->intersectCirclePathOcclusion(geoPos, pos2, 0.5);
    auto t2 = (pos2 - geoPos).length();
    if(t > 0.05 && t < t2 && geoDir*norm < 0)
    {
        auto v = norm*(1/t);
        Line3d line({
            pos,
            pos + v.to3()
        });
        line.init(scene);
        line.setInFront(true);
        line.draw();
        return v;
    }
    return { 0, 0 };
}


Vector2 Tank::separate()
{
    Vector2 sum = { 0, 0 };
    for(auto entity : scene->getEntities())
    {
        if(entity != this)
        {
            auto pos1 = geoPos, pos2 = entity->geoPos;
            auto e = (pos1 - pos2);
            auto l = std::max(1.15f, e.length());
            // Ranking units by their address and only having lower-ranked units yield way seems
            // to give a bit better result when many units try to get through a narrow gap

            // TODO: Although if a unit is in the middle of a bunch of units and you try to move
            // it then units of a greater rank won't move aside and it can get stuck, so maybe
            // only apply rank-based precedence if both units have conflicting velocity targets
            if(l < 1.5)
                sum += std::min(1.0f/std::pow(l-1, 5.0f), maxSpeed)*e.normalized();
        }
    }
    return sum;
}


Vector2 Tank::boidCalc()
{
    auto ret = evade() + seek() + avoid() + separate();
    return ret;
}
