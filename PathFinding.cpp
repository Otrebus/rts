#include "PathFinding.h"
#include <thread>
#include <chrono>


using namespace std::literals::chrono_literals;


std::mutex requestMutex;
std::mutex resultMutex;

std::queue<PathFindingRequest*> requestQueue;
std::queue<PathFindingRequest*> resultQueue;


void addPathFindingRequest(PathFindingRequest* request)
{
    std::lock_guard<std::mutex> guard(requestMutex);
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
    return request;
}


void pathFindingThread()
{
    while(true)
    {
        std::cout << resultQueue.size() << " " << requestQueue.size() << std::endl;
        // This is busy-wait, maybe we can use something like a semamphore or something instead
        auto p = popPathFindingRequest();
        if(p)
        {
            auto t = p->requester->scene->getTerrain();
            auto path = findPath(t, p->start, p->dest);
            p->path = path;
            addPathFindingResult(p);
        }
    }
}


std::vector<Vector2> findPath(Terrain* terrain, Vector2 start, Vector2 destination)
{
    auto width = terrain->getWidth(), height = terrain->getHeight();

    auto time = glfwGetTime();

    std::vector<bool> V(width*height, false);
    std::vector<real> C(width*height, inf);
    std::vector<std::pair<int, int>> P(width*height);

    std::set<std::pair<real, std::pair<int, int>>> Q;

    auto [startX, startY] = terrain->getClosestAdmissible(start);
    auto [destX, destY] = terrain->getClosestAdmissible(destination);

    int startIndex = startY*width + startX;
    int endIndex = destY*width + destX;
    C[startIndex] = 0;

    Q.insert({ 0.f, { startX, startY } });
    while(!Q.empty())
    {
        auto it = Q.begin();
        auto [prio, node] = *it;
        auto [x, y] = node;
        Q.erase(it);
        auto i = x + y*width;
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
                    int hx = (destY-(y+dy));
                    int hy = (destX-(x+dx));
                    real h = std::sqrt(hx*hx + hy*hy);
                    if(auto c2 = C[i] + std::sqrt(real(dx*dx+dy*dy)); c2 < C[j])
                    {
                        P[(y+dy)*width+x+dx] = { x, y };
                        Q.erase({ C[j]+h, { x+dx, y+dy } });
                        Q.insert({ c2 + h, { x+dx, y+dy } });
                        C[j] = c2;
                    }
                }
            }
        }
    };

    std::vector<Vector2> outPath;
    if(C[destX+destY*width] < inf)
    {
        std::vector<Vector2> result = { destination };

        for(std::pair<int, int> node = { destX, destY };;)
        {
            auto [x, y] = node;
            result.push_back({ real(x), real(y) });
            if(x == startX && y == startY)
                break;
            node = P[x+y*width];
        }
        if(result.size())
            result.back() = start;
        outPath = terrain->straightenPath(result);
        //outPath = result;
        std::cout << "Constructed path in " << glfwGetTime() - time << std::endl;
    }

    return outPath;
}