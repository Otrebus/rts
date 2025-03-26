#include "GeometryUtils.h"
#include "PathFinding.h"
#include "Polysolver.h"
#include "TankWreck.h"
#include "Projectile.h"
#include "SelectionMarkerMesh.h"
#include "Harvester.h"
#include "ShapeDrawer.h"
#include "ConsoleSettings.h"
#include "FogOfWarMaterial.h"
#include "GeometryUtils.h"
#include "UserInterface.h"
#include "LambertianMaterial.h"

ConsoleVariable Harvester::maxSpeed("harvesterMaxSpeed", 2.0f);
ConsoleVariable Harvester::maxReverseSpeed("harvesterMaxReverseSpeed", 1.5f);
ConsoleVariable Harvester::maxForwardAcc("harvesterMaxForwardAcc", 0.7f);
ConsoleVariable Harvester::maxBreakAcc("harvesterMaxBreakAcc", 0.7f);

ConsoleVariable Harvester::turnRadius("harvesterTurnRadius", 1.5f);
ConsoleVariable Harvester::maxRadialAcc("harvesterMaxRadialAcc", 2.5f);

// TODO: no need to get terrain since we have scene->getTerrain()
Harvester::Harvester(Vector3 pos, Vector3 dir, Vector3 up, Terrain* terrain) : Unit(pos, dir, up), acceleration(0), terrain(terrain), constructing(false), constructionProgress(0.f)
{
    model = ModelManager::instantiateModel("harvester");
    geoPos = pos.to2();
    geoDir = dir.to2();

    model->setDirection(dir, up);
    model->setPosition(pos);

    selectionMarkerMesh = new SelectionMarkerMesh(2, 2, true);

    boundingBox = HarvesterBoundingBox;
}


void Harvester::loadModels()
{
    auto model = new Model3d();
    model->readFromFile("harvester.obj");
    ModelManager::addModel("harvester", model);

    real length = 1;

    BoundingBox bb1;

    for(auto& mesh : model->getMeshes())
    {
        for(auto& v : ((Mesh3d*)mesh)->v)
        {
            v.pos = Vector3(v.pos.x, v.pos.z, v.pos.y);
            v.normal = -Vector3(v.normal.x, v.normal.z, v.normal.y);
            bb1.c1.x = std::min(bb1.c1.x, v.pos.x);
            bb1.c1.y = std::min(bb1.c1.y, v.pos.y);
            bb1.c1.z = std::min(bb1.c1.z, v.pos.z);
            bb1.c2.x = std::max(bb1.c2.x, v.pos.x);
            bb1.c2.y = std::max(bb1.c2.y, v.pos.y);
            bb1.c2.z = std::max(bb1.c2.z, v.pos.z);
        }
    }

    auto l = (bb1.c2.x - bb1.c1.x);
    auto w = (bb1.c2.y - bb1.c1.y);
    auto h = (bb1.c2.z - bb1.c1.z);
    auto ratio = length/l;

    BoundingBox bb[3];
    int i = 0;

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
    model->init();

    auto height = h*ratio;
    auto width = w*ratio;

    HarvesterBoundingBox = BoundingBox(Vector3(-length/2, -width/2, -height/2), Vector3(length/2, width/2, height/2));
}


Harvester::~Harvester()
{
}


Entity* Harvester::spawnWreck()
{
    return nullptr;
}


void Harvester::init(Scene* scene)
{
    this->scene = scene;
    model->setScene(scene);

    destinationLine.init(scene);
    destinationLine.setColor(Vector4(0.2f, 0.7f, 0.1f, 1.0f));
    destinationLine.setInFront(true);

    selectionMarkerMesh->setScene(scene);
    selectionMarkerMesh->init(pos.to2());
}

void Harvester::updateUniforms()
{
    model->updateUniforms();
    selectionMarkerMesh->updateUniforms();
}

void Harvester::draw(Material* mat)
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
    
    model->draw(fowMaterial);

    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);

    model->draw(mat);

    if(!curBlend)
        glDisable(GL_BLEND);
    glDepthFunc(curDepthFun);
    glBlendFunc(curSrc, curDst);

    if(selected && path.size() > 0)
        destinationLine.draw();

    if(constructing)
    {
        model->tearDown(scene);
        delete mat;
        delete body;
    }
    //drawBoundingBox();
}


void Harvester::setPosition(Vector3 pos)
{
    this->pos = pos;
    model->setPosition(pos);
}


void Harvester::setDirection(Vector3 dir, Vector3 up)
{
    this->dir = dir;
    this->up = up;
    model->setDirection(dir, up);
}


real getTime(real v, real a_f, real a_r, real maxV, real d)
{
    real ret = 0;
    if(v < 0)
    {
        auto t = -v/a_r;
        ret += t;
        d += -a_r*t*t/2 - v*t;
        v = 0;
    }

    auto tm = (maxV - v)/a_f;
    auto t = -v/a_f + std::sqrt((v*v)/(a_f*a_f) + 2*d/a_f);

    if(t < tm)
    {
        return ret + t;
    }
    else
    {
        d -= a_f*tm*tm/2 + v*tm;
        ret += tm;
        v = maxV;
        return ret + t + d/maxV;
    }
}


std::pair<bool, int> getmoveDir(Vector2 dest, Vector2 geoDirection, Vector2 geoVelocity, real maxV, real a_f, real a_r, real a_b, real turnRadius)
{
    if(!dest)
        return { geoVelocity*geoDirection < 0, 0 };
    real R = turnRadius;

    Vector2 pos = -geoDirection.perp()*R;

    real v = geoDirection*geoVelocity;

    auto dir = pos.perp().normalized(); // same as geodir...?

    auto c_l = pos + dir.perp()*R;  
        
    auto C = dir%dest > 0 ? pos + dir.perp()*R : pos -dir.perp()*R;

    auto pf = C - dest.perp().normalized()*R;
    auto pb = C + dest.perp().normalized()*R;
        
    auto x = (pf-C).normalized()*(pos-C).normalized();

    x = std::acos(std::min(1.0f, std::max(-1.0f, x)));

    auto th_f = x;
    auto th_b = -(pi-x);

    if(dir%dest <= 0)
    {
        std::swap(th_f, th_b);

        auto d1 = std::abs(th_f*R);
        auto d2 = std::abs(th_b*R);

        auto t1 = getTime(v, v > 0 ? a_f : a_r, a_b, maxV, d1);
        auto t2 = getTime(-v, v > 0 ? a_r : a_f, a_b, maxV, d2);

        int t;
        // TODO: this is redundant now
        if(t1 < t2)
        {
            t = v > 0 ? -1 : 1;
        }
        else
            t = v > 0 ? -1 : 1;

        return { t1 < t2, t };
    }
    else
    {
        auto d1 = std::abs(th_f*R);
        auto d2 = std::abs(th_b*R);

        auto t1 = getTime(v, v > 0 ? a_f : a_r, a_b, maxV, d1);
        auto t2 = getTime(-v, v > 0 ? a_r : a_f, a_b, maxV, d2);

        int t;
        if(t1 < t2)
        {
            t = v > 0 ? 1 : -1;
        }
        else
            t = v > 0 ? 1 : -1;

        return { t1 < t2, t };
    }
}


void Harvester::accelerate(Vector2 velocityTarget)
{
        // TODO: what if we do a similar thing as below but for acceleration, check current acceleration dir and how we need to amend it
    accelerationTarget = velocityTarget - geoVelocity;
    if(accelerationTarget.length() < 1e-6)
    {
        turnRate = 0;
        acceleration = 0;
        return;
    }

    //if(velocityTarget.length() > 0.01 && velocityTarget.normalized()*geoDir < 0.999 && velocityTarget.normalized()*geoDir > -0.999)
    //{
    //    if(geoDir*velocityTarget > 0)
    //        turn((geoDir.perp()*(velocityTarget) > 0));
    //    else
    //        turn((geoDir.perp()*(velocityTarget) < 0));
    //}
    //else
    //    turnRate = 0;

    // The above while more correct actually works worse in practice!
    //if(velocityTarget.length() > 0.01 && velocityTarget.normalized()*geoDir < 0.999 && velocityTarget.normalized()*geoDir > -0.999)
    //    turn(geoDir%velocityTarget > 0);
    //else
    //    turnRate = 0;

    auto maxSpeed = this->maxSpeed.get<real>();
    auto maxForwardAcc = this->maxForwardAcc.get<real>();
    auto maxBreakAcc = this->maxBreakAcc.get<real>();

    //auto radialAcc = geoDir.perp()*turnRate*maxSpeed;

    //auto x = radialAcc;
    //auto v = accelerationTarget;
    //auto ct = !x ? 0 : x.normalized()*v.normalized();
    //auto projAcc = ct ? x.length()*v.normalized()/ct - x : (v*geoDir)*geoDir;

    //if(!this->isSelected())
    //    std::cout << "turnRate is " << turnRate << std::endl;

    //ShapeDrawer::drawArrow(pos, projAcc.to3(), projAcc.length(), 0.02, Vector3(1, 0, 1));

    //accelerationTarget = velocityTarget; // Just testing stuff
    //std::cout << velocityTarget <<  " " << geoDir << std::endl;

    auto [b, t] = getmoveDir(velocityTarget, geoDir, geoVelocity, maxSpeed, maxForwardAcc, maxForwardAcc/2, maxBreakAcc, turnRadius.get<real>());

    if(t)
        turn(t > 0);
    else
        turnRate = 0;

    if(b)
    {
        if(geoDir*geoVelocity >= 0.f)
        {
            //std::cout << 1;
            acceleration = std::min(accelerationTarget.length(), maxForwardAcc);
        }
        else
        {
            //std::cout << 2;
            acceleration = maxBreakAcc;
        }
    }
    else
    {
        if(geoDir*geoVelocity >= 0.f)
        {
            //std::cout << 3;
            acceleration = -maxBreakAcc;
        }
        else
        {
            //std::cout << 4;
            acceleration = -0.5f*maxForwardAcc;
        }
    }
    ShapeDrawer::drawArrow(pos, geoDir.to3(), acceleration, 0.02, Vector3(1, 0, 1));
}

void Harvester::brake()
{
    turnRate = 0;
    auto maxBreakAcc = this->maxBreakAcc.get<real>();
    acceleration = -maxBreakAcc;
}

void Harvester::turn(bool left)
{
    auto maxRadialAcc = this->maxRadialAcc.get<float>();
    auto turnRadius = this->turnRadius.get<float>();
    turnRate = std::min(geoVelocity.length()/turnRadius, maxRadialAcc/velocity.length());
    if(!left)
        turnRate = -turnRate;
}

void Harvester::update(real dt)
{
    constructionProgress += dt*0.3f;
    if(constructionProgress >= 1.0f)
    {
        constructing = false;
        constructionProgress = 1.0f;
    }
    if(constructing)
        return;

    velocityTarget = boidCalc();
    accelerate(velocityTarget);

    handleCommand(dt);

    // If we're turning past the intended direction, clamp the turning towards the direction
    auto newDir = geoDir.normalized().rotated(turnRate*dt);
    if((newDir%velocityTarget)*(geoDir%velocityTarget) < 0 && newDir*velocityTarget > 0)
    {
        newDir = velocityTarget.normalized();
        turnRate = 0;
    }
    geoDir = newDir;

    auto newGeoVelocity = geoVelocity + geoDir.normalized()*acceleration*dt;

    if((newGeoVelocity*geoDir)*(geoVelocity*geoDir) < 0)
        geoVelocity = { 0, 0 };
    else
        geoVelocity = newGeoVelocity;

    if(!velocityTarget)
        acceleration = 0;

    auto maxSpeed = this->maxSpeed.get<real>();
    auto maxReverseSpeed = this->maxReverseSpeed.get<real>();

    if(geoVelocity.length() > maxSpeed && geoVelocity*geoDir > 0)
        geoVelocity = geoVelocity.normalized()*maxSpeed;

    else if(geoVelocity.length() > maxSpeed && geoVelocity*geoDir < 0)
        geoVelocity = geoVelocity.normalized()*maxReverseSpeed;

    auto time = glfwGetTime();

    if(!pathFindingRequest && time - pathLastCalculated > pathCalculationInterval && path.size())
        addUnitPathfindingRequest(this, path.back());
}

Vector2 Harvester::calcSeekVector(Vector2 dest)
{
    if((dest-geoPos).normalized()*geoDir > 0.999)
        return geoDir;

    if((dest-geoPos).normalized()*geoDir < -0.999)
        return -geoDir;

    auto R = this->turnRadius.get<float>();
    auto pos = geoPos;
    auto dir = geoDir;

    auto c_l = pos + dir.perp()*R;
    auto c_r = pos - dir.perp()*R;
    
    if(auto a = (c_l-dest), b = (c_r-dest); a.length() < R || b.length() < R)
        return (dest-pos);

    real forwardAcc = maxForwardAcc.get<real>();
    real reverseAcc = maxForwardAcc.get<real>()*0.5;
    real breakAcc = maxBreakAcc.get<real>()*0.5;
    auto speed = geoVelocity*geoDir;

    auto maxV = maxSpeed.get<real>();
    auto maxRV = maxReverseSpeed.get<real>();

    auto leftHand = [&] () -> std::pair<real, Vector2> {
        // Case 1: left hand turn

        auto [a, b] = getTangents(c_l, R, dest);

        Vector2 v_t = (c_l - dest) % (a - dest) > 0 ? a : b;

        auto A = (pos - c_l).normalized(), B = (v_t - c_l).normalized();

        auto angle = std::acos(A*B);
        if(A%B < 0)
            angle = 2*pi - angle;

        auto t = getTime(speed, forwardAcc, breakAcc, maxV, angle*R + (v_t - dest).length());

        auto d = (dir + dir.perp()).normalized();

        return { t, d };
    };

    auto rightHand = [&] () -> std::pair<real, Vector2> {
        // Case 1: right hand turn
        auto p = getTangents(c_r, R, dest);
        auto a = p.first;
        auto b = p.second;

        auto v_t = (c_r - dest) % (a - dest) > 0 ? b : a;

        auto A = (pos - c_r).normalized();
        auto B = (v_t - c_r).normalized();

        auto angle = std::acos(A*B);
        if(A%B > 0)
            angle = 2*pi - angle;

        auto t = getTime(speed, forwardAcc, breakAcc, maxV, angle*R + (v_t - dest).length());

        auto d =  (dir - dir.perp()).normalized();

        return { t, d };
    };

    auto rightTwoPoint = [&] () -> std::pair<real, Vector2> {
        // Case 2: right two point turn
        auto v = (dest - c_l).normalized();
        auto P = c_l + v*R;

        auto P_r = P + v*R;

        auto p = getTangents(P_r, R, dest);
        auto a = p.first;
        auto b = p.second;

        auto v_t = (P_r - dest) % (a - dest) > 0 ? b : a;

        auto A = (pos-c_l).normalized(), B = (P-c_l).normalized();
        auto angle = std::acos(A*B);
        if(A%B > 0)
            angle = 2*pi - angle;

        real ret = 0.f;

        ret += getTime(-speed, reverseAcc, breakAcc, maxRV, angle*R);

        A = (v_t-P_r).normalized(), B = (P-P_r).normalized();
        angle = std::acos(A*B);
        if(A%B < 0)
            angle = 2*pi - angle;

        ret += getTime(0.f, forwardAcc, breakAcc, maxV, angle*R + (v_t - dest).length());

        return { ret, (-dir + dir.perp()).normalized() };
    };

    auto leftTwoPoint = [&] () -> std::pair<real, Vector2> {
        // Case 2: left two point turn
        auto v = (dest - c_r).normalized();
        auto P = c_r + v*R;

        auto P_l = P + v*R;

        auto p = getTangents(P_l, R, dest);
        auto a = p.first;
        auto b = p.second;

        auto v_t = (P_l - dest) % (a - dest) > 0 ? a : b;

        auto A = (pos-c_r).normalized(), B = (P-c_r).normalized();
        auto angle = std::acos(A*B);
        if(A%B < 0)
            angle = 2*pi - angle;

        real ret = 0.f;
        ret += getTime(-speed, reverseAcc, breakAcc, maxRV, angle*R);

        A = (v_t-P_l).normalized(), B = (P-P_l).normalized();
        angle = std::acos(A*B);
        if(A%B > 0)
            angle = 2*pi - angle;

        ret += getTime(0.f, forwardAcc, breakAcc, maxV, angle*R + (v_t - dest).length());

        return { ret, (-dir - dir.perp()).normalized() };
    };

    auto leftReverse = [&] () -> std::pair<real, Vector2> {
        // Case 3: reverse left turn
        auto p = getTangents(c_l, R, dest);
        auto a = p.first;
        auto b = p.second;

        auto v_t = (c_l - dest) % (a - dest) < 0 ? a : b;

        auto A = (pos - c_l).normalized();
        auto B = (v_t - c_l).normalized();

        auto angle = std::acos(A*B);
        if(A%B > 0)
            angle = 2*pi - angle;

        auto D = angle*R + (v_t - dest).length();

        auto t = getTime(-speed, reverseAcc, breakAcc, maxRV, D);

        return { t, (-dir + dir.perp()).normalized() };
    };

    auto rightReverse = [&] () -> std::pair<real, Vector2> {
        // Case 3: reverse right turn
        auto p = getTangents(c_r, R, dest);
        auto a = p.first;
        auto b = p.second;

        auto v_t = (c_r - dest) % (a - dest) < 0 ? b : a;

        auto A = (pos - c_r).normalized();
        auto B = (v_t - c_r).normalized();

        auto angle = std::acos(A*B);
        if(A%B < 0)
            angle = 2*pi - angle;

        auto D = angle*R + (v_t - dest).length();

        auto t = getTime(-speed, reverseAcc, breakAcc, maxRV, D);

        return { t, (-dir - dir.perp()).normalized() };
    };

    auto [lh, vlh] = leftHand();
    auto [rh, vrh] = rightHand();
    auto [rtp, vrtp] = rightTwoPoint();
    auto [ltp, vltp] = leftTwoPoint();
    auto [lr, vlr] = leftReverse();
    auto [rr, vrr] = rightReverse();
    real m = min(lh, rh, rtp, ltp, lr, rr);

    std::cout << lh << std::endl;
    std::cout << rh << std::endl;
    std::cout << rtp << std::endl;
    std::cout << ltp << std::endl;
    std::cout << lr << std::endl;
    std::cout << rr << std::endl;
    std::cout << "---------------------" << std::endl;

    Vector2 v;
    if(m == lh)
    {
        std::cout << 1;
        v = vlh;
    }
    if(m == rh)
    {
        std::cout << 2;
        v = vrh;
    }
    if(m == rtp)
    {
        std::cout << 3;
        v = vrtp;
    }
    if(m == ltp)
    {
        std::cout << 4;
        v = vltp;
    }
    if(m == lr)
    {
        std::cout << 5;
        v = vlr;
    }
    if(m == rr)
    {
        std::cout << 6;
        v = vrr;
    }

    return v;
}

void Harvester::handleCommand(real dt)
{
    auto time = real(glfwGetTime());
    if(commandQueue.empty())
        return;

    auto& command = commandQueue.front();

    if(auto v = std::get_if<MoveCommand>(&command))
    {
        if(!v->active)
        {
            hasFoundPath = false;
            addUnitPathfindingRequest(this, v->destination);
            v->active = true;
        }
        else
        {        
            if(!pathFindingRequest && time - pathLastCalculated > pathCalculationInterval && path.size())
                addUnitPathfindingRequest(this, path.back());
        }

        if(!path.empty())
        {
            auto target = path.front();

            auto v2 = calcSeekVector(target);
            auto l = (target - geoPos).length();
            auto R = path.size() < 2 ? getArrivalRadius(target, scene->getUnits()) : 0.5;
            if(l < R)
            {
                path.pop_front();
                if(!path.empty())
                    this->target = path.front().to3();
                else
                {
                    if(hasFoundPath && !commandQueue.empty())
                    {
                        // TODO: Sometimes the reason path is empty is because we are still waiting for a path finding result, in which
                        //       case we shouldn't clear the command queue. Right now we use the hasFoundPath to make sure we've found a
                        //       path but this is set to true when setPath is called which might not be robust
                        commandQueue.pop();
                    }
                    this->target = Vector3(0, 0, 0);
                }
            }
        }
    }
    if(auto v = std::get_if<ExtractCommand>(&command))
    {
        if(!v->active)
        {
            real minDist = inf;
            Rock* minRock = nullptr;
            for(auto e : scene->getEntities())
            {
                if(auto rock = dynamic_cast<Rock*>(e); rock)
                {
                    auto rockPos = rock->getGeoPosition();
                    auto d = (rockPos - geoPos).length();
                    if((rockPos - v->destination).length() < v->radius && d < minDist)
                    {
                        minDist = d;
                        minRock = (Rock*) e;
                    }
                }
            }
            if(minRock)
            {
                hasFoundPath = false;
                addUnitPathfindingRequest(this, minRock->getGeoPosition() + (geoPos - minRock->getGeoPosition()).normalized());
                v->active = true;
            }
        }
        else
        {
            if(!pathFindingRequest && time - pathLastCalculated > pathCalculationInterval && path.size())
                addUnitPathfindingRequest(this, path.back());
        }
    }
}

Vector2 Harvester::seek()
{
    if(!path.empty())
    {
        auto target = path.front();
        auto l = (target - geoPos).length();

        if(path.empty())
            return { 0, 0 };
        target = path.front();
        //target = path.back();

        // TODO: this could become NaN
        if(!l)
            return { 0, 0 };
        //auto v2 = (target - geoPos).normalized();
        auto v2 = calcSeekVector(target);

        auto R = path.size() < 2 ? getArrivalRadius(target, scene->getUnits()) : 0.5;
        if(l < R)
        {
            path.pop_front();
            if(!path.empty())
                this->target = path.front().to3();
            else
                this->target = Vector3(0, 0, 0);
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
        return { 0, 0 };
}

Vector2 Harvester::evade()
{
    Vector2 sum = { 0, 0 };
    for(auto unit : scene->getEntities())
    {
        if(unit != this && !dynamic_cast<Projectile*>(unit) && !dynamic_cast<Building*>(unit) && !dynamic_cast<Rock*>(unit)) // TODO: this is getting stupid
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


Vector2 Harvester::avoid()
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


Vector2 Harvester::separate()
{
    Vector2 sum = { 0, 0 };
    for(auto unit : scene->getEntities())
    {
        if(unit != this && !dynamic_cast<Projectile*>(unit) && !dynamic_cast<Rock*>(unit))
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


Vector2 Harvester::boidCalc()
{
    auto evade_ = evade(), seek_ = seek(), avoid_ = avoid(), separate_ = separate();
    
    if(boidDebug.varInt())
    {
        if(evade_)
            ShapeDrawer::drawArrow(pos, evade_.to3(), evade_.length(), 0.02f, Vector3(1, 0, 0));
        if(seek_)
            ShapeDrawer::drawArrow(pos, seek_.to3(), seek_.length(), 0.02f, Vector3(0, 1, 0));
        if(avoid_)
            ShapeDrawer::drawArrow(pos, avoid_.to3(), avoid_.length(), 0.02f, Vector3(0, 0, 1));
        if(separate_)
            ShapeDrawer::drawArrow(pos, separate_.to3(), separate_.length(), 0.02f, Vector3(1, 1, 0));
    }

    auto sum = evade_ + seek_ + avoid_ + separate_;
    return sum;
}


BoundingBox Harvester::HarvesterBoundingBox = BoundingBox();
Material* Harvester::fowMaterial = nullptr;
