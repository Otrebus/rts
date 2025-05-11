#include "GeometryUtils.h"
#include "PathFinding.h"
#include "Polysolver.h"
#include "Projectile.h"
#include "SelectionMarkerMesh.h"
#include "Harvester.h"
#include "ShapeDrawer.h"
#include "ConsoleSettings.h"
#include "FogOfWarMaterial.h"
#include "GeometryUtils.h"
#include "UserInterface.h"
#include "LambertianMaterial.h"
#include "ModelLoader.h"

ConsoleVariable Harvester::maxSpeed("harvesterMaxSpeed", 2.0f);
ConsoleVariable Harvester::maxForwardAcc("harvesterMaxForwardAcc", 0.7f);
ConsoleVariable Harvester::maxBreakAcc("harvesterMaxBreakAcc", 0.7f);

ConsoleVariable Harvester::maxTurnRate("harvesterMaxTurnRate", 1.2f*pi/4.f);
ConsoleVariable Harvester::maxRadialAcc("harvesterMaxRadialAcc", 4.f);

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
    readFromFile(model, "harvester.obj");
    if(!ModelManager::hasModel("harvester"))
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
    drawCommands();
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

void Harvester::brake()
{
    turnRate = 0;
    auto maxBreakAcc = this->maxBreakAcc.get<real>();
    acceleration = -maxBreakAcc;
}

void Harvester::turn(bool left)
{
    auto maxRadialAcc = this->maxRadialAcc.get<float>();
    auto maxTurnRate = this->maxTurnRate.get<float>();
    turnRate = std::min(maxTurnRate, maxRadialAcc/velocity.length());
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
            addUnitPathfindingRequest(this, v->destination);
            v->active = true;
        }
        else
        {        
            if(!pathFindingRequest && time - pathLastCalculated > pathCalculationInterval && path.size())
            {
                addUnitPathfindingRequest(this, path.back());
            }
            if(!pathFindingRequest && !this->target)
                commandQueue.pop();
        }
    }
    if(auto v = std::get_if<ExtractCommand>(&command))
    {
        if(v->status == ExtractCommand::None)
        {
            std::cout << "none" << std::endl;
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
                addUnitPathfindingRequest(this, minRock->getGeoPosition() + (geoPos - minRock->getGeoPosition()).normalized());
                v->active = true;
                v->rock = minRock;
                v->status = ExtractCommand::Moving;
            }
            else
            {
                std::cout << "inactive" << std::endl;
                commandQueue.pop();
                v->active = false;
            }
        }
        else if(v->status == ExtractCommand::Moving)
        {
            std::cout << "moving" << std::endl;
            if(!pathFindingRequest && time - pathLastCalculated > pathCalculationInterval && path.size())
                addUnitPathfindingRequest(this, path.back());

            if(v->rock && (geoPos - v->rock->getGeoPosition()).length() < 3.0f)
                v->status = ExtractCommand::Rotating;
            else
                v->status = ExtractCommand::Moving;
        }
        else if(v->status == ExtractCommand::Rotating)
        {
            std::cout << "rotating" << std::endl;
            if(((geoDir*(v->rock->getGeoPosition() - geoPos).normalized()) < 0.90))
                turn(geoDir%(geoPos - v->rock->getGeoPosition()) < 0);
            else
            {
                v->status = ExtractCommand::Extracting;
            }
        }
        else if(v->status == ExtractCommand::Extracting)
        {
            std::cout << "extracting" << std::endl;
            if(glfwGetTime() - v->lastSpawnedParticle > 0.02f)
            {
                auto gp = new ConstructionParticle(pos + dir*0.5f, *v->rock, true);
                v->lastSpawnedParticle = glfwGetTime();
                scene->addParticle(gp);
            }
            v->rock->health -= 50*dt;
            if(v->rock->health < .0f) {                
                v->rock->setDead();
                v->status = ExtractCommand::None;
            }
        }
    }
}

Vector2 Harvester::seek()
{
    if(!path.empty())
    {
        auto target = path.front();
        auto l = (target - geoPos).length();

        target = path.front();

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

Vector2 Harvester::evade()
{
    Vector2 sum = { 0.f, 0.f };
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


BoundingBox Harvester::HarvesterBoundingBox = BoundingBox();
Material* Harvester::fowMaterial = nullptr;
