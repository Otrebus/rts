#include "GeometryUtils.h"
#include "PathFinding.h"
#include "Polysolver.h"
#include "TankWreck.h"
#include "Projectile.h"
#include "SelectionMarkerMesh.h"
#include "Vehicle.h"
#include "ShapeDrawer.h"
#include "ConsoleSettings.h"
#include "FogOfWarMaterial.h"
#include "GeometryUtils.h"
#include "LambertianMaterial.h"

ConsoleVariable Vehicle::vehicleMaxSpeed("vehicleMaxSpeed", 2.0f);
ConsoleVariable Vehicle::vehicleMaxForwardAcc("vehicleMaxForwardAcc", 0.7f);
ConsoleVariable Vehicle::vehicleMaxBreakAcc("vehicleMaxBreakAcc", 0.7f);

ConsoleVariable Vehicle::vehicleMaxTurnAngle("vehicleMaxTurnAngle", 1.2*pi/4);
ConsoleVariable Vehicle::vehicleMaxRadialAcc("vehicleMaxRadialAcc", 4.f);

// TODO: no need to get terrain since we have scene->getTerrain()
Vehicle::Vehicle(Vector3 pos, Vector3 dir, Vector3 up, Terrain* terrain) : Unit(pos, dir, up), acceleration(0), terrain(terrain), constructing(false), constructionProgress(0.0f)
{
}


Vehicle::~Vehicle()
{
}


Entity* Vehicle::spawnWreck()
{
}


void Tank::init(Scene* scene)
{
    this->scene = scene;
}

void Vehicle::updateUniforms()
{
}

void Vehicle::draw(Material* mat)
{
    GLint curDepthFun;
    GLboolean curBlend;
    GLint curSrc, curDst;

    glGetIntegerv(GL_DEPTH_FUNC, &curDepthFun);
    glGetBooleanv(GL_BLEND, &curBlend);
    glGetIntegerv(GL_BLEND_SRC_RGB, &curSrc);
    glGetIntegerv(GL_BLEND_DST_RGB, &curDst);

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
    
    glBlendFuncSeparate(GL_ZERO, GL_ONE, GL_ONE, GL_ZERO);
    
    /*body->draw(fowMaterial);
    */

    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);

    /*body->draw(mat);
    */

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
    //drawBoundingBox();
}


void Vehicle::setPosition(Vector3 pos)
{
    this->pos = pos;
    //body->setPosition();
}


void Vehicle::setDirection(Vector3 dir, Vector3 up)
{
    this->dir = dir;
    this->up = up;
    //body->setDirection(dir, up);
}


void Vehicle::accelerate(Vector2 velocityTarget)
{
    accelerationTarget = velocityTarget - geoVelocity;
    if(accelerationTarget.length() < 1e-6)
    {
        //turnRate = 0;
        acceleration = 0;
        return;
    }

    /*if(velocityTarget.length() > 0.01 && velocityTarget.normalized()*geoDir < 0.999)
        turn(geoDir%velocityTarget > 0);
    else
        turnRate = 0;*/

    auto maxSpeed = this->vehicleMaxSpeed.get<real>();
    auto maxForwardAcc = this->vehicleMaxForwardAcc.get<real>();
    auto maxBreakAcc = this->vehicleMaxBreakAcc.get<real>();

    //auto radialAcc = geoDir.perp()*turnRate*maxSpeed;

    //auto x = radialAcc;
    //auto v = accelerationTarget;
    //auto ct = !x ? 0 : x.normalized()*v.normalized();
    //auto projAcc = ct ? x.length()*v.normalized()/ct - x : (v*geoDir)*geoDir;

    //if(projAcc*geoDir < 0 && geoDir*geoVelocity <= 0) // We don't want to reverse at maxBreakAcc
    //    acceleration = 0;
    //else
    //    acceleration = std::max(-maxBreakAcc, std::min(maxForwardAcc, projAcc*geoDir));
}

void Vehicle::brake()
{
    /*turnRate = 0;
    auto maxBreakAcc = this->tankMaxBreakAcc.get<real>();*/
    acceleration = -maxBreakAcc;
}

void Vehicle::turn(bool left)
{
    /*auto maxRadialAcc = this->tankMaxRadialAcc.get<float>();
    auto maxTurnRate = this->tankMaxTurnRate.get<float>();
    turnRate = std::min(maxTurnRate, maxRadialAcc/velocity.length());*/
    if(!left)
        turnRate = -turnRate;
}

void Vehicle::update(real dt)
{
    constructionProgress += dt*0.3;
    if(constructionProgress >= 1.0f)
    {
        constructing = false;
        constructionProgress = 1.0f;
    }
    if(constructing)
        return;


    velocityTarget = boidCalc();
    accelerate(velocityTarget);

    //auto newDir = geoDir.normalized().rotated(turnRate*dt);
    //if((newDir%velocityTarget)*(geoDir%velocityTarget) < 0 && newDir*velocityTarget > 0)
    //    newDir = velocityTarget.normalized();
    //geoDir = newDir;

    //geoVelocity += geoDir.normalized()*acceleration*dt;

    //if(!velocityTarget)
    //{
    //    // NOTE: if we don't check for velocity1*geoVelocity < 0, then the tanks will back up initially
    //    acceleration = 0;
    //}

    auto maxSpeed = this->vehicleMaxSpeed.get<real>();
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
}

Vector2 Vehicle::seek()
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

            auto maxSpeed = this->vehicleMaxSpeed.get<real>();
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

Vector2 Vehicle::evade()
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


Vector2 Vehicle::avoid()
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


Vector2 Vehicle::separate()
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

            auto maxSpeed = this->vehicleMaxSpeed.get<real>();
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


Vector2 Vehicle::boidCalc()
{
    auto evade_ = evade(), seek_ = seek(), avoid_ = avoid(), separate_ = separate();
    
    if(boidDebug.varInt())
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


BoundingBox Vehicle::vehicleBoundingBox = BoundingBox();
Material* Vehicle::fowMaterial = nullptr;
