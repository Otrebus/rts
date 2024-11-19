#include "GeometryUtils.h"
#include "PathFinding.h"
#include "Polysolver.h"
#include "TankWreck.h"
#include "Projectile.h"
#include "SelectionMarkerMesh.h"
#include "Tank.h"
#include "ShapeDrawer.h"
#include "ConsoleSettings.h"
#include "FogOfWarMaterial.h"

ConsoleVariable Tank::boidDebug("boidDebug", 0);

void Tank::loadModels()
{
    auto body = new Model3d();
    body->readFromFile("tankbody.obj");

    auto turret = new Model3d();
    turret->readFromFile("tankturret.obj");

    auto gun = new Model3d();
    gun->readFromFile("tankbarrel.obj");

    ModelManager::addModel("tankbody", body);
    ModelManager::addModel("tankturret", turret);
    ModelManager::addModel("tankbarrel", gun);

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
    tankBoundingBox = BoundingBox(Vector3(-length/2, -width/2, -height/2), Vector3(length/2, width/2, height/2));

    body->init();
    gun->init();
    turret->init();
}


// TODO: no need to get terrain since we have scene->getTerrain()
Tank::Tank(Vector3 pos, Vector3 dir, Vector3 up, Terrain* terrain) : Unit(pos, dir, up), turnRate(0), acceleration(0), terrain(terrain), gunRecoilPos(0.0f)
{
    body = ModelManager::instantiateModel("tankbody");
    turret = ModelManager::instantiateModel("tankturret");
    gun = ModelManager::instantiateModel("tankbarrel");

    if(!fowMaterial)
        fowMaterial = new FogOfWarMaterial();

    turretDir = Vector3(1, 1, 0).normalized();

    turretTarget = Vector3(-1, 1, 1).normalized();
    gunPos = Vector3(0, 0.18, 0);
    turretPos = Vector3(0, 0, 0.18);

    geoPos = pos.to2();
    geoDir = dir.to2();

    health = 100;

    lastFired = -inf;

    boundingBox = tankBoundingBox;

    selectionMarkerMesh = new SelectionMarkerMesh(this);

    pathCalculationInterval = (500 + (rand() % 500))/1000.0f;

    for(auto model :{ body, turret, gun })
    {
        model->setPosition(pos);
        model->setDirection(dir, up);
        //model->init(); // TODO: we're updating the VAOs etc, but not deleting the old ones
    }
}


Tank::~Tank()
{
}


Entity* Tank::spawnWreck()
{
    auto tw = new TankWreck(pos, dir, up, terrain);
    tw->body->setPosition(this->body->getPosition());
    tw->gun->setPosition(this->gun->getPosition());
    tw->turret->setPosition(this->turret->getPosition());

    tw->absGunPos = absGunPos;
    tw->absGunDir = absGunDir;
    tw->absGunUp = absGunUp;

    tw->absTurPos = absTurPos;
    tw->absTurUp = absTurUp;
    tw->absTurDir = absTurDir;

    tw->init(scene);
    return tw;
}


void Tank::init(Scene* scene)
{
    this->scene = scene;
    body->setScene(scene);
    turret->setScene(scene);
    gun->setScene(scene);
    destinationLine.init(scene);
    //destinationLine.init(scene);
    destinationLine.setColor(Vector3(0.2, 0.7, 0.1));
    destinationLine.setInFront(true);

    //enemyLine.init(scene);
    //enemyLine.setColor(Vector3(0.2, 0.7, 0.1));
    //enemyLine.setInFront(true);

    selectionMarkerMesh->setScene(scene);
    selectionMarkerMesh->init();
}

void Tank::updateUniforms()
{
    body->updateUniforms();
    turret->updateUniforms();
    gun->updateUniforms();
    selectionMarkerMesh->updateUniforms();
}

void Tank::drawTurret(Material* mat)
{
    turret->setDirection(absTurDir, absTurUp);
    turret->setPosition(absTurPos);

    gun->setDirection(absGunDir, absGunUp);
    gun->setPosition(absGunPos);

    turret->draw(mat);
    gun->draw(mat);
}

void Tank::updateTurret(real dt)
{
    gunRecoilPos = std::max(gunRecoilPos - gunRecoilRecoveryRate*dt, 0.0f);
    auto u = turretDir.to2().normalized(), v = turretTarget.to2().normalized();

    absTurPos = pos + (dir*turretPos.y + (dir%up).normalized()*turretPos.x + up*turretPos.z);

    absTurUp = up;
    absTurDir = (dir*turretDir.y + (dir%up).normalized()*turretDir.x).normalized();
    absGunDir = dir*turretDir.y + up*turretDir.z + (dir%up).normalized()*turretDir.x;

    absGunPos = absTurPos + absTurDir*gunPos.y + (absTurDir%up).normalized()*gunPos.x + gunPos.z*absTurUp - absGunDir*gunRecoilPos;
    absGunUp = (absGunDir%(absTurDir%up)).normalized();
    absMuzzlePos = absGunPos + absGunDir*gunLength;

    if(std::abs(std::acos(u*v)) < dt*turretYawRate)
        u = v;
    else
        u = u.rotated((u%v > 0 ? 1 : -1)*dt*turretYawRate);

    auto targetTheta = std::max(std::asin(-0.1f), std::asin(turretTarget.z));
    auto turretTheta = std::asin(turretDir.z);
    real theta;

    if(std::abs(targetTheta - turretTheta) < dt*turretPitchRate)
        theta = targetTheta;
    else
        theta = turretTheta+dt*turretPitchRate*sgn(targetTheta-turretTheta);

    auto x = std::cos(theta), y = std::sin(theta);

    turretDir = Vector3(x*u.x, x*u.y, y);
}

void Tank::draw(Material* mat)
{
    GLint curDepthFun;
    GLboolean curBlend;
    GLint curSrc, curDst;

    glGetIntegerv(GL_DEPTH_FUNC, &curDepthFun);
    glGetBooleanv(GL_BLEND, &curBlend);
    glGetIntegerv(GL_BLEND_SRC_RGB, &curSrc);
    glGetIntegerv(GL_BLEND_DST_RGB, &curDst);

    std::vector<Vector3> P;
    // TODO: slow
    P.push_back(getPosition());
    for(auto p : path)
        P.push_back({ p.x, p.y, terrain->getElevation(p.x, p.y) });
    destinationLine.setVertices(P);
    
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

    if(selected && path.size() > 0)
        destinationLine.draw();
}


void Tank::shoot()
{
    auto p = new Projectile(absMuzzlePos, absGunDir, absTurUp, this);
    p->setVelocity(absGunDir.normalized()*bulletSpeed + velocity);
    p->init(scene);
    scene->addEntity(p);

    for(int i = 0; i < 150; i++)
    {
        auto gp = new GunFireParticle(absMuzzlePos, absGunDir, velocity);
        scene->addParticle(gp);
    }

    gunRecoilPos = gunRecoil;
}


void Tank::setPosition(Vector3 pos)
{
    this->pos = pos;
    body->setPosition(pos);
}


void Tank::setDirection(Vector3 dir, Vector3 up)
{
    this->dir = dir;
    this->up = up;
    body->setDirection(dir, up);
}


void Tank::accelerate(Vector2 velocityTarget)
{
    accelerationTarget = velocityTarget - geoVelocity;
    if(accelerationTarget.length() < 1e-6)
    {
        turnRate = 0;
        acceleration = 0;
        return;
    }

    if(velocityTarget.length() > 0.01 && velocityTarget.normalized()*geoDir < 0.999)
        turn(geoDir%velocityTarget > 0);
    else
        turnRate = 0;

    auto radialAcc = geoDir.perp()*turnRate*maxSpeed;

    auto x = radialAcc;
    auto v = accelerationTarget;
    auto ct = !x ? 0 : x.normalized()*v.normalized();
    auto projAcc = ct ? x.length()*v.normalized()/ct - x : (v*geoDir)*geoDir;

    if(projAcc*geoDir < 0 && geoDir*geoVelocity <= 0) // We don't want to reverse at maxBreakAcc
        acceleration = 0;
    else
        acceleration = std::max(-maxBreakAcc, std::min(maxForwardAcc, projAcc*geoDir));
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

bool Tank::setBallisticTarget(Unit* enemyTarget)
{
    auto g = Vector3(0, 0, -gravity);
    auto v = enemyTarget->velocity - velocity;
    auto p = enemyTarget->pos - absMuzzlePos; // NOTE: this isn't quite right

    ld t4 = g*g/4;
    ld t3 = -v*g;
    ld t2 = -p*g + v*v - bulletSpeed*bulletSpeed;
    ld t1 = 2.0f*p*v;
    ld c = p*p;

    auto ts = findPolynomialRoots({ t4, t3, t2, t1, c });

    for(real t : ts)
    {
        if(t > 0)
        {
            auto target = p + v*t - g*t*t/2.f;
            if(canTurretAbsoluteTarget(target))
            {
                setTurretAbsoluteTarget(target);
                return true;
            }
        }
    }

    return false;
}

void Tank::update(real dt)
{
    Unit* closestEnemy = nullptr;
    real closestD = inf;
    for(auto unit : scene->getUnits())
    {
        if(unit->isEnemy() != enemy && !unit->dead)
        {
            if(auto d = (unit->getPosition() - pos).length(); d < closestD)
            {
                closestD = d;
                closestEnemy = unit;
            }
        }
    }

    velocityTarget = boidCalc();
    accelerate(velocityTarget);

    auto newDir = geoDir.normalized().rotated(turnRate*dt);
    if((newDir%velocityTarget)*(geoDir%velocityTarget) < 0 && newDir*velocityTarget > 0)
        newDir = velocityTarget.normalized();
    geoDir = newDir;

    geoVelocity += geoDir.normalized()*acceleration*dt;

    if(!velocityTarget)
    {
        // NOTE: if we don't check for velocity1*geoVelocity < 0, then the tanks will back up initially
        acceleration = 0;
    }

    if(geoVelocity.length() > maxSpeed)
        geoVelocity = geoVelocity.normalized()*maxSpeed;

    auto time = glfwGetTime();

    if(!pathFindingRequest && time - pathLastCalculated > pathCalculationInterval && path.size())
    {
        PathFindingRequest* request = new PathFindingRequest;
        request->requester = this;
        request->start = getPosition().to2();
        request->dest = path.back();
        setCurrentPathfindingRequest(request);
        addPathFindingRequest(request);
    }


    auto enemyTarget = dynamic_cast<Unit*>(scene->getEntity(enemyTargetId));

    if(closestEnemy && (!enemyTarget || closestD*0.95 < (enemyTarget->getPosition()-this->getPosition()).length()))
    {
        enemyTargetId = closestEnemy->getId();
        enemyTarget = (Unit*) scene->getEntity(enemyTargetId);
    }
    
    if(!enemyTarget || enemyTarget && (enemyTarget->dead || !setBallisticTarget(enemyTarget)))
    {
        enemyTargetId = 0;
        enemyTarget = nullptr;
        turretTarget = Vector3(0, 1, 0);
    }

    updateTurret(dt);

    if(enemyTarget && time - lastFired > fireInterval)
    {
        bool isInFog = false;
        // TODO: the way we handle enemy fog of war isn't correct and needs an update when
        // we have separate fow for player and enemy
        auto p = (enemyTarget->isEnemy() ? enemyTarget : this)->getGeoPosition();
        isInFog = scene->getTerrain()->getFog(p.x, p.y) && scene->getTerrain()->fogOfWarEnabled.var;
        if(!isInFog && (turretTarget*turretDir) > 1-1e-6)
        {
            lastFired = time;
            shoot();
            auto light = new PointLight(time);

            light->setPos(absMuzzlePos);
            light->setVelocity(this->velocity);
            scene->addLight(light);
        }
    }
}

Vector2 Tank::seek()
{
    if(!path.empty())
    {
        auto target = path.front();

        if(target.length() > 0.0001)
        {
            auto l = (target - geoPos).length();

            if(path.empty())
                return { 0, 0 };
            target = path.front();
            //target = path.back();

            // TODO: this could become NaN
            if(!l)
                return { 0, 0 };
            auto v2 = (target - geoPos).normalized();

            auto R = path.size() < 2 ? getArrivalRadius(target, scene->getUnits()) : 0.5;
            if(l < R)
            {
                path.pop_front();
                if(!path.empty())
                    this->target = path.front().to3();
                else
                    this->target = Vector3(0, 0, 0);
            }

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
    Vector2 sum = { 0, 0 };
    for(auto unit : scene->getEntities())
    {
        if(unit != this && !dynamic_cast<Projectile*>(unit) && !dynamic_cast<Building*>(unit)) // TODO: this is getting stupid
        {
            auto pos1 = geoPos, pos2 = unit->geoPos;
            auto v1 = geoVelocity, v2 = unit->getGeoVelocity();
            auto e = pos2 - pos1;
            auto v = v2 - v1;

            if(e*v >= 0)
                continue;

            auto costh = -e*v/(e.length()*v.length());
            auto th = std::acos(costh);
            auto y = tan(th)*e.length();

            auto d = (2.f-y);
            if(d <= 0 || !v || e.length()/v.length() > 2.5f)
                continue;

            auto t = (e.length()/(costh*v.length()));

            auto s = e.perp().normalized()*d/t;

            if(v%e > 0)
                sum += s;
            else
                sum -= s;
        }
    }
    return sum;
}


Vector2 Tank::avoid()
{
    auto pos2 = geoPos + geoVelocity;

    auto [t, norm] = terrain->intersectCirclePathOcclusion(geoPos, pos2, 0.5);
    auto t2 = (pos2 - geoPos).length();
    if(t > 0.05 && t < t2 && geoDir*norm < 0)
    {
        auto v = norm*(1/t);
        return v;
    }
    return { 0, 0 };
}


Vector2 Tank::separate()
{
    Vector2 sum = { 0, 0 };
    for(auto unit : scene->getEntities())
    {
        if(unit != this && !dynamic_cast<Projectile*>(unit))
        {
            auto pos1 = geoPos, pos2 = unit->geoPos;
            if(pos1 == pos2)
                continue;
            auto e = (pos1 - pos2);
            auto l = std::max(1.15f, e.length());
            // Ranking units by their address and only having lower-ranked units yield way seems
            // to give a bit better result when many units try to get through a narrow gap

            // TODO: Although if a unit is in the middle of a bunch of units and you try to move
            // it then units of a greater rank won't move aside and it can get stuck, so maybe
            // only apply rank-based precedence if both units have conflicting velocity targets
            if(l < 1.55)
                sum += std::min(1.0f/std::pow(l-0.245f, 3.0f), maxSpeed)*e.normalized();
            else
                sum += std::min(std::max(2-l, 0.0f), maxSpeed)*e.normalized();
        }
    }
    return sum;
}


Vector2 Tank::boidCalc()
{
    auto evade_ = evade(), seek_ = seek(), avoid_ = avoid(), separate_ = separate();
    
    if(boidDebug.var)
    {
        if(evade_)
            ShapeDrawer::drawArrow(pos, evade_.to3(), evade_.length(), 0.02, Vector3(1, 0, 0));
        if(seek_)
            ShapeDrawer::drawArrow(pos, seek_.to3(), seek_.length(), 0.02, Vector3(0, 1, 0));
        if(avoid_)
            ShapeDrawer::drawArrow(pos, avoid_.to3(), avoid_.length(), 0.02, Vector3(0, 0, 1));
        if(separate_)
            ShapeDrawer::drawArrow(pos, separate_.to3(), separate_.length(), 0.02, Vector3(1, 1, 0));
    }

    auto sum = evade_ + seek_ + avoid_ + separate_;
    return sum;
}


void Tank::setTurretAbsoluteTarget(Vector3 target)
{
    assert(target.to2());
    target.normalize();
    turretTarget = rebaseOrtho(target, dir%up, dir, up);
}


bool Tank::canTurretAbsoluteTarget(Vector3 target)
{
    assert(target.to2());
    target.normalize();
    turretTarget = rebaseOrtho(target, dir%up, dir, up);

    if(turretTarget.z < std::asin(minTurretPitch) || turretTarget.z > std::asin(maxTurretPitch))
        return false;
    return true;
}


real Tank::gunLength = 1.0f;
BoundingBox Tank::tankBoundingBox = BoundingBox();
Material* Tank::fowMaterial = nullptr;