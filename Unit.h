#pragma once
#pragma once

#define NOMINMAX
#include "Vector3.h"
#include <vector>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include "Scene.h"
#include "ShaderProgram.h"
#include "BoundingBox.h"
#include "Mesh3d.h"
#include "LineMesh3d.h"
#include "Entity.h"
#include <deque>

class SelectionMarkerMesh;
class BoundingBoxModel;
class Model3d;
class Scene;
class PathFindingRequest;

class Unit : public Entity, public std::enable_shared_from_this<Unit>
{
public:
	Unit(Vector3 pos, Vector3 dir, Vector3 up);
    ~Unit();

    void setTarget(Vector3 pos);
    Vector3 getTarget() const;

    PathFindingRequest* getCurrentPathfindingRequest() const;
    void setCurrentPathfindingRequest(PathFindingRequest* request);

    void setPath(std::deque<Vector2> path);

    void drawSelectionDecal(int pass);

    bool isSelected() const;
    void setSelected(bool selected);
    void setPreSelected(bool preSelected);

    void setScene(Scene* s);

    void setEnemy(bool enemy);
    bool isEnemy() const;

    void setEnemyTarget(Unit* enemy);
    Unit* getEnemyTarget() const;

    real health;

protected:
    PathFindingRequest* pathFindingRequest;
    real pathLastCalculated;
    real pathCalculationInterval;
    bool selected;
    bool preSelected;
    std::deque<Vector2> path;
    SelectionMarkerMesh* selectionMarkerMesh;

    bool enemy;
    Unit* enemyTarget;
};
