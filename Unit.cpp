#include "SelectionDecalMaterial.h"
#include "SelectionMarkerMesh.h"
#include <array>
#include "Vector3.h"
#include "Line.h"
#include "Vector2.h"
#include <cassert>
#include "Terrain.h"
#include <vector>

ConsoleVariable Unit::boidDebug("boidDebug", 0);

Unit::Unit(Vector3 pos, Vector3 dir, Vector3 up) : Entity(pos, dir, up) {}

Unit::~Unit()
{
}

PathFindingRequest* Unit::getCurrentPathfindingRequest() const
{
    return pathFindingRequest;
}

void Unit::setCurrentPathfindingRequest(PathFindingRequest* request)
{
    this->pathFindingRequest = request;
}


void Unit::setTarget(Vector3 target)
{
    //this->target = target;
}

Vector3 Unit::getTarget() const
{
    //return target;
    // TODO: not really used
    return { 0.f, 0.f, 0.f };
}

void Unit::setPath(std::deque<Vector2> path)
{
    if(!commandQueue.empty())
    {
        // This if statement is a relatively lame attempt at preventing a path find update to return
        // after a command has been finished and still set a path. It would be more robust if the path
        // finding request stored some unique id of the command that sent it and check that it's still
        // in the queue; now we just check that there's any command there
        hasFoundPath = true;
        this->path = std::deque(path.begin(), path.end());
        pathLastCalculated = real(glfwGetTime());
    }
}

void Unit::setPreSelected(bool preSelected)
{
    this->preSelected = preSelected;
}


void Unit::drawCommands()
{
    if(!selected)
        return;

    if(commandQueue.empty())
        return;

    std::vector<Vector2> P = { getGeoPosition() };
        
    std::vector<Vector3> S;

    for(auto command : commandQueue.get())
    {
        if(auto v = std::get_if<MoveCommand>(&command))
        {
            P.push_back(v->destination);
        }
        else if(auto v = std::get_if<BuildCommand>(&command))
        {
            auto w = v->width, h = v->height;
            auto pos = v->destination;
            Line3d buildingLine;

            std::vector<Vector2> vs = {
                pos + Vector2(0, 0),
                pos + Vector2(w, 0),
                pos + Vector2(w, h),
                pos + Vector2(0, h),
                pos + Vector2(0, 0)
            };

            std::vector<Vector3> S;
            for(auto v : vs)
                S.push_back( { v.x, v.y, scene->getTerrain()->getElevation(v.x, v.y) });

            buildingLine.init(scene);
            buildingLine.setVertices(S);
            buildingLine.setColor(Vector4(0.2f, 0.7f, 0.1f, 0.5f));
            buildingLine.setInFront(true);
            buildingLine.draw();
            P.push_back(v->destination + Vector2(w/2, h/2));
        }
        else if(auto v = std::get_if<ExtractCommand>(&command))
        {
            P.push_back(v->destination);
            auto pos = v->destination;
            Line3d extractionLine;

            std::vector<Vector3> vs;

            real segmentWidth = 0.4;
            const int N = std::max(20, int(v->radius*2*pi/segmentWidth));
            for(int i = 0; i < N+1; i++)
            {
                auto w = v->destination + v->radius*Vector2(std::cos(i*2*pi/N), std::sin(i*2*pi/N));
                vs.push_back(Vector3(w.x, w.y, scene->getTerrain()->getElevation(w.x, w.y)));
            }

            extractionLine.init(scene);
            extractionLine.setVertices(vs);
            extractionLine.setColor(Vector4(0.2f, 0.7f, 0.1f, 0.5f));
            extractionLine.setInFront(true);
            extractionLine.draw();
        }
    }
    
    Line3d queueLine;

    std::reverse(P.begin(), P.end());

    std::vector<Vector3> P3;
    for(auto p : P)
        P3.push_back( { p.x, p.y, scene->getTerrain()->getElevation(p.x, p.y) });

    queueLine.setDashed(true);
    queueLine.init(scene);
    queueLine.setVertices(P3);
    queueLine.setColor(Vector4(0.2f, 0.7f, 0.1f, 0.5f));
    queueLine.setInFront(true);
    queueLine.draw();
}


void Unit::drawSelectionDecal(int pass)
{
    if(selected || preSelected)
    {
        selectionMarkerMesh->update(getGeoPosition());
        selectionMarkerMesh->pass = pass;
        if(isEnemy())
            ((SelectionDecalMaterial*)(selectionMarkerMesh->getMaterial()))->Kd = Vector3(0.8f, 0.f, 0.f);
        selectionMarkerMesh->updateUniforms();
        selectionMarkerMesh->draw();
        selectionMarkerMesh->setSelectionType(preSelected && !selected);
    }
}

void Unit::setScene(Scene* scene)
{
    this->scene = scene;
    selectionMarkerMesh->setScene(scene);
}

void Unit::setSelected(bool selected)
{
    this->selected = selected;
}

bool Unit::isSelected() const
{
    return selected;
}

bool Unit::isEnemy() const
{
    return enemy;
}

void Unit::setEnemy(bool enemy)
{
    this->enemy = enemy;
}

void Unit::setEnemyTargetId(int enemy)
{
    this->enemyTargetId = enemy;
}

int Unit::getEnemyTargetId() const
{
    return enemyTargetId;
}

ConsoleVariable Unit::drawPaths("drawPaths", 0);
