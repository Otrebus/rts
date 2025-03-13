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
    destinationLine.setColor(Vector3(0.2f, 0.7f, 0.1f));
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
    //    {
    //        turn((geoDir.perp()*(velocityTarget) > 0));
    //    }
    //    else
    //    {
    //        turn((geoDir.perp()*(velocityTarget) < 0));
    //    }
    //}
    //else
    //    turnRate = 0;

    // The above while more correct actually works worse in practice!
    if(velocityTarget.length() > 0.01 && velocityTarget.normalized()*geoDir < 0.999 && velocityTarget.normalized()*geoDir > -0.999)
        turn(geoDir%velocityTarget > 0);
    else
        turnRate = 0;

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

    //if(projAcc*geoDir < 0 && geoDir*geoVelocity <= 0)
    //    acceleration = -maxForwardAcc*0.5f;
    //else
    //    acceleration = projAcc*geoDir > 0 ? maxForwardAcc : - maxBreakAcc;

    if(accelerationTarget*geoDir > 0)
        acceleration = maxForwardAcc;
    else
        acceleration = -maxForwardAcc*0.5f;

    if(!isSelected())
        std::cout << acceleration << std::endl;
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

    auto time = glfwGetTime();

    if(!pathFindingRequest && time - pathLastCalculated > pathCalculationInterval && path.size())
    {
        addUnitPathfindingRequest(this, path.back());
    }
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

    real reverseCost = 1.5;

    auto leftHand = [&] () -> std::pair<real, Vector2> {
        // Case 1: left hand turn

        auto [a, b] = getTangents(c_l, R, dest);

        Vector2 v_t = (c_l - dest) % (a - dest) > 0 ? a : b;

        auto A = (pos - c_l).normalized(), B = (v_t - c_l).normalized();

        auto angle = std::acos(A*B);
        if(A%B < 0)
            angle = 2*pi - angle;

        return { angle*R + (v_t - dest).length(), (dir + dir.perp()).normalized() };
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

        return { angle*R + (v_t - dest).length(), (dir - dir.perp()).normalized() };
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

        real ret = 0;

        ret += reverseCost*angle*R;

        A = (v_t-P_r).normalized(), B = (P-P_r).normalized();
        angle = std::acos(A*B);
        if(A%B < 0)
            angle = 2*pi - angle;

        return { ret + angle*R + (v_t - dest).length(), (-dir -dir.perp()).normalized() };
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

        real ret = reverseCost*angle*R;

        A = (v_t-P_l).normalized(), B = (P-P_l).normalized();
        angle = std::acos(A*B);
        if(A%B > 0)
            angle = 2*pi - angle;

        return { ret + angle*R + (v_t - dest).length(), (-dir + dir.perp()).normalized() };
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

        return { reverseCost*D, (-dir - dir.perp()).normalized() };
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

        auto D_2 = angle*R + (v_t - dest).length();

        return { reverseCost*D_2, (-dir + dir.perp()).normalized() };
    };

    auto [lh, vlh] = leftHand();
    auto [rh, vrh] = rightHand();
    auto [rtp, vrtp] = rightTwoPoint();
    auto [ltp, vltp] = leftTwoPoint();
    auto [lr, vlr] = leftReverse();
    auto [rr, vrr] = rightReverse();
    real m = min(lh, rh, rtp, ltp, lr, rr);

    Vector2 v;
    if(m == lh)
        v = vlh;
    if(m == rh)
        v = vrh;
    if(m == rtp)
        v = vrtp;
    if(m == ltp)
        v = vltp;
    if(m == lr)
        v = vlr;
    if(m == rr)
        v = vrr;

    return v;
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
        return -geoVelocity;
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
