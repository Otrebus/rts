#include "GeometryUtils.h"
#include "PathFinding.h"
#include "Polysolver.h"
#include "TankWreck.h"
#include "UserInterface.h"
#include "LineModelMaterial.h"
#include "Projectile.h"
#include "SelectionMarkerMesh.h"
#include "Tank.h"
#include <variant>
#include "ShapeDrawer.h"
#include "ConsoleSettings.h"
#include "FogOfWarMaterial.h"
#include "GeometryUtils.h"
#include "LambertianMaterial.h"
#include "Logger.h"

ConsoleVariable Tank::maxSpeed("tankMaxSpeed", 2.0f);
ConsoleVariable Tank::maxForwardAcc("tankMaxForwardAcc", 0.7f);
ConsoleVariable Tank::maxBreakAcc("tankMaxBreakAcc", 0.7f);

ConsoleVariable Tank::turretYawRate("tankTurretYawRate", pi/4);
ConsoleVariable Tank::turretPitchRate("tankTurretPitchRate", 0.25*pi/4);

ConsoleVariable Tank::minTurretPitch("tankMinTurretPitch", -10_deg);
ConsoleVariable Tank::maxTurretPitch("tankMaxTurretPitch", 90_deg);

ConsoleVariable Tank::bulletSpeed("tankBulletSpeed", 5.0);
ConsoleVariable Tank::fireInterval("tankFireInterval", 1.5);

ConsoleVariable Tank::maxTurnRate("tankMaxTurnRate", 1.2*pi/4);
ConsoleVariable Tank::maxRadialAcc("tankMaxRadialAcc", 4.f);

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

    BoundingBox bb1;

    for(auto model : { body, turret, gun })
    {
        for(auto& mesh : model->getMeshes())
        {
            for(auto& v : ((Mesh3d*)mesh)->v)
            {
                v.pos = Vector3(v.pos.z, v.pos.x, v.pos.y);
                v.normal = Vector3(v.normal.z, v.normal.x, v.normal.y);
                bb1.c1.x = std::min(bb1.c1.x, v.pos.x);
                bb1.c1.y = std::min(bb1.c1.y, v.pos.y);
                bb1.c1.z = std::min(bb1.c1.z, v.pos.z);
                bb1.c2.x = std::max(bb1.c2.x, v.pos.x);
                bb1.c2.y = std::max(bb1.c2.y, v.pos.y);
                bb1.c2.z = std::max(bb1.c2.z, v.pos.z);
            }
        }
    }

    auto l = (bb1.c2.x - bb1.c1.x);
    auto w = (bb1.c2.y - bb1.c1.y);
    auto h = (bb1.c2.z - bb1.c1.z);
    auto ratio = length/l;

    BoundingBox bb[3];
    int i = 0;
    for(auto model : { body, turret, gun })
    {
        for(auto& mesh : model->getMeshes())
        {
            for(auto& v : ((Mesh3d*)mesh)->v)
            {
                bb[i].c1.x = std::min(bb[i].c1.x, v.pos.x);
                bb[i].c1.y = std::min(bb[i].c1.y, v.pos.y);
                bb[i].c1.z = std::min(bb[i].c1.z, v.pos.z);
                bb[i].c2.x = std::max(bb[i].c2.x, v.pos.x);
                bb[i].c2.y = std::max(bb[i].c2.y, v.pos.y);
                bb[i].c2.z = std::max(bb[i].c2.z, v.pos.z);
            }
        }
        for(auto& mesh : model->getMeshes())
        {
            for(auto& v : ((Mesh3d*)mesh)->v)
            {
                v.pos -= Vector3((bb[i].c2.x + bb[i].c1.x)/2, (bb[i].c2.y + bb[i].c1.y)/2, (bb[i].c2.z + bb[i].c1.z)/2);
                v.pos *= length/l;
            }
        }
        i++;
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
    auto width = w*ratio;
    tankBoundingBox = BoundingBox(Vector3(-length/2, -width/2, -height/2), Vector3(length/2, width/2, height/2));

    body->init();
    gun->init();
    turret->init();
}


// TODO: no need to get terrain since we have scene->getTerrain()
Tank::Tank(Vector3 pos, Vector3 dir, Vector3 up, Terrain* terrain) : Unit(pos, dir, up), turnRate(0), acceleration(0), terrain(terrain), gunRecoilPos(0.f), constructing(false), constructionProgress(0.f)
{
    body = ModelManager::instantiateModel("tankbody");
    turret = ModelManager::instantiateModel("tankturret");
    gun = ModelManager::instantiateModel("tankbarrel");

    if(!fowMaterial)
        fowMaterial = new FogOfWarMaterial();

    turretDir = Vector3(1, 0, 0).normalized();

    turretTarget = Vector3(1, 0, 0).normalized();

    geoPos = pos.to2();
    geoDir = dir.to2();

    health = 100;

    lastFired = -inf;

    boundingBox = tankBoundingBox;

    selectionMarkerMesh = new SelectionMarkerMesh(2, 2, true);

    pathCalculationInterval = (500 + (rand() % 500))/1000.f;

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
    destinationLine.setColor(Vector4(0.2f, 0.7f, 0.1f, 1.0f));
    destinationLine.setInFront(true);

    //enemyLine.init(scene);
    //enemyLine.setColor(Vector3(0.2, 0.7, 0.1));
    //enemyLine.setInFront(true);

    selectionMarkerMesh->setScene(scene);
    selectionMarkerMesh->init(pos.to2());
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
    this->turret->setDirection(absTurDir, absTurUp);
    this->turret->setPosition(absTurPos - up*turretPos.z/2);

    this->gun->setDirection(absGunDir, absGunUp);
    this->gun->setPosition(absGunPos - up*turretPos.z/2);

    if(constructing)
    {
        auto z = (tankBoundingBox.c2 - tankBoundingBox.c1).z;
        auto p = pos - z/2*up;

        //auto turret = splitModelIntoLineModel(*this->turret, p + z*up*constructionProgress, -up);
        //auto gun = splitModelIntoLineModel(*this->gun, p + z*up*constructionProgress, -up);

        auto turret = splitModelAsConstructing(*this->turret, z, pos, up, constructionProgress);
        auto gun = splitModelAsConstructing(*this->gun, z, pos, up, constructionProgress);


        turret->setScene(scene);
        gun->setScene(scene);

        turret->init();
        gun->init();

        turret->draw(mat);
        gun->draw(mat);

        turret->tearDown(scene);
        gun->tearDown(scene);

        delete turret;
        delete gun;
    }
    else
    {
        this->turret->draw(mat);
        this->gun->draw(mat);
    }
}

void Tank::updateTurret(real dt)
{
    gunRecoilPos = std::max(gunRecoilPos - gunRecoilRecoveryRate*dt, 0.f);
    auto u = turretDir.to2().normalized(), v = turretTarget.to2().normalized();

    absTurPos = pos + (dir*turretPos.x + (dir%up).normalized()*turretPos.y + up*turretPos.z);

    absTurUp = up;
    absTurDir = (dir*turretDir.x + (dir%up).normalized()*turretDir.y).normalized();
    absGunDir = dir*turretDir.x + up*turretDir.z + (dir%up).normalized()*turretDir.y;

    absGunPos = absTurPos + absTurDir*gunPos.x + (absTurDir%up).normalized()*gunPos.y + gunPos.z*absTurUp - absGunDir*gunRecoilPos;
    absGunUp = (absGunDir%(absTurDir%up)).normalized();
    absMuzzlePos = absGunPos + absGunDir*gunLength;

    auto turretYawRate = this->turretYawRate.get<real>();

    if(std::abs(std::acos(u*v)) < dt*turretYawRate)
        u = v;
    else
        u = u.rotated((u%v > 0 ? 1 : -1)*dt*turretYawRate);

    auto targetTheta = std::max(std::asin(-0.1f), std::asin(turretTarget.z));
    auto turretTheta = std::asin(turretDir.z);
    real theta;

    auto turretPitchRate = this->turretPitchRate.get<real>();

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

    if(drawPaths.varInt())
    {
        std::vector<Vector2> P = { getGeoPosition() };
        for(auto p : path)
            P.push_back(p);
        std::vector<Vector3> S;
        // TODO: slow
        for(int i = 0; i+1 < P.size(); i++)
        {
            auto pieces = terrain->chopLine(P[i], P[i+1]);
            S.insert(S.end(), pieces.begin(), pieces.end());
        }
        destinationLine.setVertices(S);
    }
    
    glBlendFuncSeparate(GL_ZERO, GL_ONE, GL_ONE, GL_ZERO);
    
    Model3d* body;
    if(constructing)
    {
        auto z = (tankBoundingBox.c2 - tankBoundingBox.c1).z;
        auto p = pos - z/2*up;

        body = splitModelAsConstructing(*this->body, z, pos, up, constructionProgress);
        body->init();
        body->setScene(scene);
    }
    else
        body = this->body;

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

    if(constructing)
    {
        body->tearDown(scene);
        delete mat;
        delete body;
    }
    drawCommands();
}


void Tank::shoot()
{
    auto p = new Projectile(absMuzzlePos, absGunDir, absTurUp, this);
    auto bulletSpeed = this->bulletSpeed.get<real>();
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
    body->setPosition(pos-up*turretPos.z/2);
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

    auto maxSpeed = this->maxSpeed.get<real>();
    auto maxForwardAcc = this->maxForwardAcc.get<real>();
    auto maxBreakAcc = this->maxBreakAcc.get<real>();

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
    auto maxBreakAcc = this->maxBreakAcc.get<real>();
    acceleration = -maxBreakAcc;
}

void Tank::turn(bool left)
{
    auto maxRadialAcc = this->maxRadialAcc.get<float>();
    auto maxTurnRate = this->maxTurnRate.get<float>();
    turnRate = std::min(maxTurnRate, maxRadialAcc/velocity.length());
    if(!left)
        turnRate = -turnRate;
}

bool Tank::setBallisticTarget(Unit* enemyTarget)
{
    auto g = Vector3(0, 0, -gravity);
    auto v = enemyTarget->velocity - velocity;
    auto p = enemyTarget->pos - absMuzzlePos; // NOTE: this isn't quite right

    auto bulletSpeed = this->bulletSpeed.get<real>();

    ld t4 = g*g/4;
    ld t3 = -v*g;
    ld t2 = -p*g + v*v - bulletSpeed*bulletSpeed;
    ld t1 = 2.0f*p*v;
    ld c = p*p;

    auto ts = findPolynomialRoots({ t4, t3, t2, t1, c });

    for(auto T : ts)
    {
        real t = real(T);
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

void Tank::handleCommand(real)
{
    auto time = real(glfwGetTime());
    if(commandQueue.empty())
        return;

    auto& command = commandQueue.front();

    if(auto v = std::get_if<MoveCommand>(&command))
    {
        if(!v->active)
        {
            addUnitPathfindingRequest(this, v->destination);
            v->active = true;
        }
        else
        {        
            if(!pathFindingRequest && time - pathLastCalculated > pathCalculationInterval && path.size())
            {
                addUnitPathfindingRequest(this, path.back());
            }
        }
    }
    else
    {
    }
}

void Tank::update(real dt)
{
    auto time = real(glfwGetTime());
    updateTurret(dt);
    constructionProgress += dt*0.1f;
    if(constructionProgress >= 1.0f)
    {
        constructing = false;
        constructionProgress = 1.0f;
    }
    if(constructing)
        return;

    handleCommand(dt);

    Unit* closestEnemy = nullptr;
    real closestD = inf;
    for(auto unit : scene->getUnits())
    {
        if(unit->isEnemy() != enemy && !unit->dead)
        {
            // TODO: only works for friendly players, enemy tanks will track no matter what and get the jump

            // TODO: break this (fow check) into a separate function
            bool isInFog = false;
            if(dynamic_cast<Tank*>(unit))
                isInFog = scene->getTerrain()->getFog(int(unit->getGeoPosition().x), int(unit->getGeoPosition().y)) && scene->getTerrain()->fogOfWarEnabled.varInt();
            else if(Building* building = dynamic_cast<Building*>(unit); building)
            {
                int width = scene->getTerrain()->getHeight();
                for(auto p : building->getAbsoluteFootprint())
                {
                    int y = p/width;
                    int x = p%width;
                    isInFog = scene->getTerrain()->getFog(x, y) && scene->getTerrain()->fogOfWarEnabled.varInt();
                    if(!isInFog)
                        break;
                }
            }
            if(isInFog)
                continue;
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
        acceleration = 0;
    }

    auto maxSpeed = this->maxSpeed.get<real>();
    if(geoVelocity.length() > maxSpeed)
        geoVelocity = geoVelocity.normalized()*maxSpeed;

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
        turretTarget = Vector3(1, 0, 0);
    }

    auto fireInterval = this->fireInterval.get<real>();

    if(enemyTarget && time - lastFired > fireInterval)
    {
        bool isInFog = false;
        // TODO: the way we handle enemy fog of war isn't correct and needs an update when
        // we have separate fow for player and enemy
        auto p = (enemyTarget->isEnemy() ? enemyTarget : this)->getGeoPosition();
        if(dynamic_cast<Tank*>(enemyTarget))
            isInFog = scene->getTerrain()->getFog(int(p.x), int(p.y)) && scene->getTerrain()->fogOfWarEnabled.varInt();
        else if(Building* building = dynamic_cast<Building*>(enemyTarget); building)
        {
            int width = scene->getTerrain()->getHeight();
            for(auto p : building->getAbsoluteFootprint())
            {
                int y = p/width;
                int x = p%width;
                isInFog = scene->getTerrain()->getFog(x, y) && scene->getTerrain()->fogOfWarEnabled.varInt();
                if(!isInFog)
                    break;
            }
        }
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
        auto l = (target - geoPos).length();

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
            {
                if(!commandQueue.empty())
                    commandQueue.pop();
                this->target = Vector3(0, 0, 0);
            }
        }

        auto maxSpeed = this->maxSpeed.get<real>();
        auto speed = maxSpeed;
        if(path.size() == 1)
        {
            if((target - geoPos).length() < 0.5)
                return { 0, 0 };
            speed = std::min(maxSpeed, (target - geoPos).length());
        }
        return v2*speed;
    }
    else
        return { 0.f, 0.f };
}

Vector2 Tank::evade()
{
    Vector2 sum = { 0.f, 0.f };
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
    if(!geoVelocity)
        return { 0, 0 };
    auto pos2 = geoPos + geoVelocity.normalized();
    //ShapeDrawer::drawArrow(pos, (pos2 - pos.to2()).to3().normalized(), (pos2.to3() - pos).length(), 0.1, Vector3(1, 0, 0));

    auto [t, norm] = terrain->intersectCirclePathOcclusion(geoPos, pos2, 0.5);

    auto t2 = (pos2 - geoPos).length();
    if(t > 0.05 && geoDir*norm < 0)
    {
        auto v = t2*norm*(1/t);
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

            auto maxSpeed = this->maxSpeed.get<real>();
            // Ranking units by their address and only having lower-ranked units yield way seems
            // to give a bit better result when many units try to get through a narrow gap

            // TODO: Although if a unit is in the middle of a bunch of units and you try to move
            // it then units of a greater rank won't move aside and it can get stuck, so maybe
            // only apply rank-based precedence if both units have conflicting velocity targets
            if(l < 1.55)
                sum += std::min(1.0f/std::pow(l-0.245f, 3.0f), maxSpeed)*e.normalized();
            else
                sum += std::min(std::max(2-l, 0.f), maxSpeed)*e.normalized();
        }
    }
    return sum;
}


Vector2 Tank::boidCalc()
{
    auto evade_ = evade(), seek_ = seek(), avoid_ = avoid(), separate_ = separate();
    
    if(boidDebug.varInt())
    {
        if(evade_)
            ShapeDrawer::drawArrow(pos, evade_.to3(), evade_.length(), 0.02f, Vector3(1.f, 0.f, 0.f));
        if(seek_)
            ShapeDrawer::drawArrow(pos, seek_.to3(), seek_.length(), 0.02f, Vector3(0.f, 1.f, 0.f));
        if(avoid_)
            ShapeDrawer::drawArrow(pos, avoid_.to3(), avoid_.length(), 0.02f, Vector3(0.f, 0.f, 1.f));
        if(separate_)
            ShapeDrawer::drawArrow(pos, separate_.to3(), separate_.length(), 0.02f, Vector3(1.f, 1.f, 0.f));
    }

    auto sum = evade_ + seek_ + avoid_ + separate_;
    return sum;
}


void Tank::setTurretAbsoluteTarget(Vector3 target)
{
    assert(target.to2());
    target.normalize();
    turretTarget = rebaseOrtho(target, dir, dir%up, up);
}


bool Tank::canTurretAbsoluteTarget(Vector3 target)
{
    assert(target.to2());
    target.normalize();
    turretTarget = rebaseOrtho(target, dir, dir%up, up);

    auto minTurretPitch = this->minTurretPitch.get<real>();
    auto maxTurretPitch = this->maxTurretPitch.get<real>();

    if(turretTarget.z < std::asin(minTurretPitch) || turretTarget.z > std::asin(maxTurretPitch))
        return false;
    return true;
}


real Tank::gunLength = 1.0f;
BoundingBox Tank::tankBoundingBox = BoundingBox();
Material* Tank::fowMaterial = nullptr;
const Vector3 Tank::gunPos = Vector3(0.18f, 0.f, 0.f);
const Vector3 Tank::turretPos = Vector3(0.f, 0.f, 0.18f);
