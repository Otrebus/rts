#define NOMINMAX
#include "Entity.h"
#include "LambertianMaterial.h"
#include "Line.h"
#include "LineMaterial.h"
#include "LineMesh3d.h"
#include "Mesh3d.h"
#include "Model3d.h"
#include "Ray.h"
#include "SelectionDecalMaterial.h"
#include "SelectionMarkerMesh.h"
#include "Terrain.h"
#include "Vertex3d.h"
#include <array>
#include <vector>

Unit::Unit(Vector3 pos, Vector3 dir, Vector3 up) : Entity(pos, dir, up) {}

Unit::~Unit()
{
    std::cout << "Deleting unit" << std::endl;
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
    return { 0, 0, 0 };
}

void Unit::setPath(std::deque<Vector2> path)
{
    this->path = std::deque(path.begin(), path.end());
    pathLastCalculated = glfwGetTime();
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
        if(isEnemy())
            ((SelectionDecalMaterial*)(selectionMarkerMesh->getMaterial()))->Kd = Vector3(0.8, 0, 0);
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
