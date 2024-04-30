#pragma once

#include <vector>
#include "Vector2.h"
#include "Terrain.h"
#include "Entity.h"
#include "Utils.h"
#include "TerrainMesh.h"
#include "Unit.h"
#include "Model3d.h"
#include "Vector3.h"
#include "TerrainMaterial.h"
#include "Math.h"
#include <array>
#include "Ray.h"
#include <set>
#include <algorithm>
#include "Math.h"
#include <queue>
#include <mutex>

class PathFindingRequest
{
public:
    Entity* requester;

    Vector2 start;
    Vector2 dest;

    std::vector<Vector2> path;
};


void addPathFindingRequest(PathFindingRequest*);
void addPathFindingResult(PathFindingRequest*);
std::vector<Vector2> findPath(Terrain* terrain, Vector2 start, Vector2 destination);

PathFindingRequest* popPathFindingResult();
PathFindingRequest* popPathFindingRequest();

void pathFindingThread();