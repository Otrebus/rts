#pragma once
#pragma once

#define NOMINMAX
#include "BoundingBox.h"
#include "Entity.h"
#include "LineMesh3d.h"
#include "Mesh3d.h"
#include "Scene.h"
#include "ShaderProgram.h"
#include "Vector3.h"
#include <deque>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <vector>

class SelectionMarkerMesh;
class BoundingBoxModel;
class Model3d;
class Scene;
class PathFindingRequest;

class Unit : public Entity
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

    virtual Entity* spawnWreck() = 0;

    void setEnemyTargetId(int enemy);
    int getEnemyTargetId() const;

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
    int enemyTargetId;
};
