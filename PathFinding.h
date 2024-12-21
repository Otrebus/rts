#pragma once

#include "Terrain.h"
#include "Unit.h"
#include <queue>

class PathFindingRequest
{
public:
    Unit* requester;

    Vector2 start;
    Vector2 dest;

    std::deque<Vector2> path;
};

class PriorityQueue
{
    struct PriorityQueueEntry
    {
        real prio;
        int key;
    };
    std::vector<PriorityQueueEntry> A;
    std::vector<int> P;
    int n;
    void heapify(int pos);

public:
    PriorityQueue(int size);
    ~PriorityQueue();

    int size();
    void insert(int key, real priority);
    void decreaseKey(int key, real priority);
    int pop();
    bool empty();
};

void addPathFindingRequest(PathFindingRequest*);
void addPathFindingResult(PathFindingRequest*);
std::deque<Vector2> findPath(Terrain* terrain, Vector2 start, Vector2 destination);

PathFindingRequest* popPathFindingResult();
PathFindingRequest* popPathFindingRequest();

void pathFindingThread();

extern std::mutex pathFindingMutex;
