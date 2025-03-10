#pragma once

#include "Entity.h"
#include <deque>
#include "ConsoleSettings.h"
#include "Command.h"
#include <queue>
#include <optional>

class SelectionMarkerMesh;
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

    void drawCommands();

    virtual Entity* spawnWreck() = 0;

    void setEnemyTargetId(int enemy);
    int getEnemyTargetId() const;

    real health;
    
    static ConsoleVariable boidDebug;
    static ConsoleVariable drawPaths;

    CommandQueue commandQueue;

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
