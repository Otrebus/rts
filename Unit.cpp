#define NOMINMAX
#include "Entity.h"
#include "Mesh3d.h"
#include "Vertex3d.h"
#include "Model3d.h"
#include "LineMesh3d.h"
#include "Line.h"
#include "Ray.h"
#include "Terrain.h"
#include "LambertianMaterial.h"
#include "LineMaterial.h"
#include <vector>
#include <array>
#include "BoundingBoxModel.h"
#include "SelectionMarkerMesh.h"

Unit::Unit(Vector3 pos, Vector3 dir, Vector3 up) : Entity(pos, dir, up) {}


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
    return { 0, 0, 0 };
}

void Unit::setPath(std::vector<Vector2> path)
{
    this->path = path;
    pathLastCalculated = glfwGetTime();
}

const std::vector<Vector2>& Unit::getPath() const
{
    return path;
}

void Unit::setPreSelected(bool preSelected)
{
    this->preSelected = preSelected;
}


void Unit::drawSelectionDecal(int pass)
{
    if(selected || preSelected)
    {
        selectionMarkerMesh->update();
        selectionMarkerMesh->pass = pass;
        selectionMarkerMesh->draw();
        selectionMarkerMesh->setSelectionType(preSelected && !selected);
    }
}

void Unit::init(Scene* scene)
{
    this->scene = scene;
    selectionMarkerMesh->init(scene);
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


void Unit::setEnemyTarget(Unit* enemy)
{
    this->enemyTarget = enemy;
}


Unit* Unit::getEnemyTarget() const
{
    return enemyTarget;
}
