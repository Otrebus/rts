#include "Main.h"
#include "PathFinding.h"
#include "GeometryUtils.h"
#include <chrono>
#include <thread>

using namespace std::literals::chrono_literals;

std::mutex requestMutex;
std::mutex resultMutex;
std::mutex pathFindingMutex;

std::queue<PathFindingRequest*> requestQueue;
std::queue<PathFindingRequest*> resultQueue;

PriorityQueue::PriorityQueue(int size)
{
    int k = 1 << int(1 + std::log2(size+1.01));
    A.resize(k);
    P.resize(size+1);
    A[0] = { 0, 0 };
    n = 0;
}

PriorityQueue::~PriorityQueue()
{
}

int PriorityQueue::size()
{
    return n;
}

bool PriorityQueue::empty()
{
    return !n;
}

void PriorityQueue::heapify(int key)
{
    for(int p = P[key]; p && A[p/2].prio > A[p].prio; p /= 2)
    {
        std::swap(P[A[p].key], P[A[p/2].key]);
        std::swap(A[p], A[p/2]);
    }
}

void PriorityQueue::insert(int key, real prio)
{
    A[++n] = { prio, key };
    P[key] = n;
    heapify(key);
}

void PriorityQueue::decreaseKey(int key, real prio)
{
    if(P[key])
    {
        int p = P[key];
        A[p].prio = prio;
        heapify(key);
    }
    else
        insert(key, prio);
}

int PriorityQueue::pop()
{
    int key = A[1].key, k = 1;
    std::swap(P[key], P[A[n].key]);
    std::swap(A[k], A[n]);

    n--;
    while(k*2 <= n)
    {
        int R = k*2 + 1, L = k*2;
        int s = (R <= n && A[R].prio < A[L].prio) ? R : L;
        if(A[s].prio >= A[k].prio)
            break;

        std::swap(P[A[s].key], P[A[k].key]);
        std::swap(A[k], A[s]);
        k = s;
    }

    P[key] = 0;
    return key;
}

void addPathFindingRequest(PathFindingRequest* request)
{
    std::lock_guard<std::mutex> guard(requestMutex);
    request->requester->scene->borrow(request->requester);
    requestQueue.push(request);
}

void addPathFindingResult(PathFindingRequest* request)
{
    std::lock_guard<std::mutex> guard(resultMutex);
    resultQueue.push(request);
}

PathFindingRequest* popPathFindingRequest()
{
    std::lock_guard<std::mutex> guard(requestMutex);
    if(requestQueue.empty())
        return nullptr;
    auto request = requestQueue.front();
    requestQueue.pop();
    return request;
}

PathFindingRequest* popPathFindingResult()
{
    std::lock_guard<std::mutex> guard(resultMutex);
    if(resultQueue.empty())
        return nullptr;

    auto request = resultQueue.front();
    resultQueue.pop();
    request->requester->scene->unBorrow(request->requester);
    return request;
}

void pathFindingThread()
{
    while(!quitting)
    {
        // This is busy-wait, maybe we can use something like a semaphore or something instead
        auto p = popPathFindingRequest();
        if(p)
        {
            auto t = (p->requester)->scene->getTerrain();
            auto path = findPath(t, p->requester->geoPos, p->dest);
            p->path = path;
            addPathFindingResult(p);
        }
    }
}

std::deque<Vector2> findPath(Terrain* terrain, Vector2 start, Vector2 destination)
{
    std::lock_guard<std::mutex> guard(pathFindingMutex);
    auto width = terrain->getWidth(), height = terrain->getHeight();

    //auto time = glfwGetTime();

    std::vector<bool> V(width*height, false);
    std::vector<real> C(width*height, inf);
    std::vector<std::pair<int, int>> P(width*height);
    std::fill(P.begin(), P.end(), std::make_pair(-1, -1 ));

    std::pair<int, int> minQi;
    real minQ = inf;

    PriorityQueue Q(width*height);

    auto [startX, startY] = terrain->getClosestAdmissible(start);
    auto [destX, destY] = terrain->getClosestAdmissible(destination);

    int startIndex = startY*width + startX;
    int endIndex = destY*width + destX;
    C[startIndex] = 0;

    auto dist = [](int x, int y) { return real(std::sqrt(real(x*x + y*y))); };

    Q.insert(startY*width+startX, 0);

    while(!Q.empty())
    {
        auto i = Q.pop();
        int x = i%width, y = i/width;
        V[i] = true;
        if(i == endIndex)
            break;

        for(int dx = -1; dx <= 1; dx++)
        {
            for(int dy = -1; dy <= 1; dy++)
            {
                int j = (y+dy)*width + x+dx;
                if((dx || dy) && terrain->inBounds(x+dx, y+dy) && !V[j] && terrain->isAdmissible(x+dx, y+dy))
                {
                    real h_j = dist(destX-(x+dx), destY-(y+dy));
                    real d = dist(dx, dy);

                    if(auto c2 = C[i] + d; c2 < C[j])
                    {
                        if(minQ > h_j)
                        {
                            minQ = h_j;
                            minQi = { x+dx, y+dy };
                        }
                        C[j] = c2;
                        Q.decreaseKey(j, C[j] + h_j);
                        P[j] = { x, y };
                    }
                }
            }
        }
    };

    std::deque<Vector2> outPath;

    if(C[destX+destY*width] == inf && minQ < inf)
    {
        destX = minQi.first;
        destY = minQi.second;
        destination = { real(destX), real(destY) };
    }

    std::deque<Vector2> result = { destination };

    for(std::pair<int, int> node = { destX, destY };;)
    {
        auto [x, y] = node;
        result.push_back({ real(x), real(y) });
        if(x == startX && y == startY || x == -1 && y == -1)
            break;
        node = P[x+y*width];
    }

    std::reverse(result.begin(), result.end());
    outPath = terrain->straightenPath(result.begin(), result.end());
    return outPath;
}
