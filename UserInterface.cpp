#define NOMINMAX
#include "Entity.h"
#include "InputManager.h"
#include "Line.h"
#include "Math.h"
#include "PathFinding.h"
#include "Ray.h"
#include "Tank.h"
#include "Unit.h"
#include "UserInterface.h"
#include <numeric>
#include <random>
#include <ranges>
#include "LambertianMaterial.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

UserInterface::UserInterface(GLFWwindow* window, Scene* scene, CameraControl* cameraControl) : scene(scene), cameraControl(cameraControl), cursor(nullptr), window(window), lastClickedUnit(nullptr)
{
    drawBoxc1 = { 0, 0 };
    drawBoxc2 = { 0, 0 };
    selectState = NotSelecting;
    intersecting = false;
    selectingAdditional = false;
    showConsole = false;
    console = new Console(scene);
    console->init();
}


struct Polyhedra
{
    std::vector<Vector3> vertices;
    std::vector<std::pair<int, int>> edges;
    std::vector<int> faceIndices;
    std::vector<std::pair<int, int>> faces;
};

int sideSAT(const Polyhedra& a, Ray r)
{
    int neg = 0, pos = 0;
    for(auto v : a.vertices)
    {
        float t = 0;
        if(v == r.pos)
            pos++, neg++;
        else
        {
            t = (v-r.pos).normalized()*r.dir;
            if(t > -eps)
                pos++;
            if(t < eps)
                neg++;
        }
    }
    return neg == a.vertices.size() ? -1 : pos == a.vertices.size() ? 1 : 0;
}

bool testSides(const Polyhedra& a, const Polyhedra& b, const Ray& ray)
{
    auto sa = sideSAT(a, ray);
    auto sb = sideSAT(b, ray);
    return sa*sb < 0;
}

bool testNormals(const Polyhedra& a, const Polyhedra& b)
{
    auto& v = a.vertices;
    auto& f = a.faceIndices;
    for(auto& [i, n] : a.faces)
    {
        auto v1 = (v[f[i]] - v[f[i+1]]).normalized();
        auto v2 = (v[f[i+2]] - v[f[i]]).normalized();
        if(auto k = v1%v2; k && testSides(a, b, { v[f[i+1]], (v1%v2).normalized() }))
            return false;
    }
    return true;
}

bool intersectsSAT(const Polyhedra& a, const Polyhedra& b)
{
    std::vector<Ray> P;

    if(!testNormals(a, b))
        return false;
    if(!testNormals(b, a))
        return false;

    for(auto [a1, a2] : a.edges)
    {
        for(auto [b1, b2] : b.edges)
        {
            auto u = a.vertices[a1]-a.vertices[a2];
            auto v = b.vertices[b1]-b.vertices[b2];
            auto n = u%v;
            if(!!n && testSides(a, b, { a.vertices[a1], n.normalized() }))
                return false;
        }
    }
    return true;
}

bool intersectsFrustum(Vector3 pos, Vector3 v[4], Entity& entity, Scene* scene)
{
    auto M = getDirectionMatrix(entity.dir, entity.up);

    auto [c1, c2] = entity.boundingBox;

    Vector3 C[8] = {
        c1,
        Vector3(c1.x, c2.y, c1.z),
        Vector3(c2.x, c2.y, c1.z),
        Vector3(c2.x, c1.y, c1.z),
        Vector3(c1.x, c1.y, c2.z),
        Vector3(c2.x, c1.y, c2.z),
        c2,
        Vector3(c1.x, c2.y, c2.z),
    };

    for(auto& c : C)
        c = M*c + entity.pos;

    int ci[24] = {
        0, 1, 2, 3,
        0, 3, 5, 4,
        3, 2, 6, 5,
        2, 1, 7, 6,
        1, 0, 4, 7,
        4, 5, 6, 7
    };


    Polyhedra a = {
        std::vector<Vector3>(C, C+8),
        { { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 }, { 4, 5 }, { 5, 6 }, { 6, 7 }, { 7, 4 }, { 0, 4 }, { 3, 5 }, { 2, 6 }, { 1, 7 } },
        {
            0, 1, 2, 3,
            0, 3, 5, 4,
            3, 2, 6, 5,
            2, 1, 7, 6,
            1, 0, 4, 7,
            4, 5, 6, 7
        },
        { { 0, 4 }, { 4, 4 }, { 8, 4 }, { 12, 4 }, { 16, 4 }, { 20, 4 } }
    };

    Polyhedra b = {
        { pos, pos + v[0]*1.e3f, pos + v[1]*1.e3f, pos + v[2]*1.e3f, pos + v[3]*1.e3f },
        { { 0, 1 }, { 0, 2 }, { 0, 3 }, { 0, 4 }, { 1, 2 }, { 2, 3 }, { 3, 4 }, { 4, 1 } },
        { 0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 1  },
        { { 0, 3 }, { 3, 3 }, { 6, 3 }, { 9, 3 } }
    };

    return intersectsSAT(a, b);
}


void addUnitPathfindingRequest(Unit* unit, Vector3 pos)
{
    PathFindingRequest* request = new PathFindingRequest;
    request->requester = unit;
    request->start = unit->getPosition().to2();
    request->dest = pos.to2();
    addPathFindingRequest(request);
    unit->setCurrentPathfindingRequest(request);
}


void UserInterface::selectUnits(std::vector<Unit*> units, bool pre)
{
    auto time = glfwGetTime();
    auto camera = scene->getCamera();

    auto c1 = drawBoxc1, c2 = drawBoxc2;

    auto tr = Vector2(std::max(c1.x, c2.x), std::max(c1.y, c2.y));
    auto tl = Vector2(std::min(c1.x, c2.x), std::max(c1.y, c2.y));
    auto bl = Vector2(std::min(c1.x, c2.x), std::min(c1.y, c2.y));
    auto br = Vector2(std::max(c1.x, c2.x), std::min(c1.y, c2.y));

    Vector3 v[4] = {
        camera->getViewRay(tr.x, tr.y).dir,
        camera->getViewRay(tl.x, tl.y).dir,
        camera->getViewRay(bl.x, bl.y).dir,
        camera->getViewRay(br.x, br.y).dir
    };

    for(auto u : units)
    {
        if(true)//if(!u->isEnemy())
        {
            if(!pre)
            {
                if(intersectsFrustum(camera->getPos(), v, *u, scene))
                    u->setSelected(true); // TODO: clean
            }
            else
            {
                if(intersectsFrustum(camera->getPos(), v, *u, scene))
                    u->setPreSelected(true); // TODO: clean
            }
        }
    }
}

bool UserInterface::isInFrustum(Unit* unit)
{
auto time = glfwGetTime();
    auto camera = scene->getCamera();

    Vector3 v[4] = {
        camera->getViewRay(1, 1).dir,
        camera->getViewRay(-1, 1).dir,
        camera->getViewRay(-1, -1).dir,
        camera->getViewRay(1, -1).dir
    };

    return intersectsFrustum(camera->getPos(), v, *unit, scene);
}

Unit* UserInterface::getUnit(const Ray& ray, const std::vector<Unit*>& units) const
{
    for(auto u : units)
        if(u->intersectBoundingBox(ray))
            return u;
    return nullptr;
}

void UserInterface::moveDrawnUnits(const std::vector<Unit*>& selectedUnits)
{
    real length = 0;
    for(int i = 0; i < drawTarget.size()-1; i++)
        length += (drawTarget[i+1]-drawTarget[i]).length();

    auto k = length/2;
    std::vector<Vector3> points;
    if(selectedUnits.size() > 1)
    {
        k = length/(selectedUnits.size()-1);
        points.insert(points.end(), { drawTarget.front(), drawTarget.back() });
    }

    real L = 0, K = k;
    for(int i = 0; i < drawTarget.size()-1; i++)
    {
        auto l = (drawTarget[i+1]-drawTarget[i]).length();
        L += l;
        while(L >= K && points.size() < selectedUnits.size())
        {
            auto p = drawTarget[i] + (drawTarget[i+1]-drawTarget[i])*((K-(L-l))/l);
            points.push_back(p);
            K += k;
        }
    }

    // Match units to the points (to minimize the total length we'd need the Hungarian algorithm)
    std::vector<bool> P(points.size(), false);

    struct Assignment
    {
        Unit* unit;
        int point;
        real d;
    };

    std::vector<Assignment> v;

    for(auto u : selectedUnits)
    {
        real min = inf;
        int mini = 0;
        for(int i = 0; i < points.size(); i++)
        {
            if(auto d = (points[i] - u->getPosition()).length(); !P[i] && d < min)
            {
                min = d;
                mini = i;
            }
        }

        v.push_back(Assignment{ u, mini, min });
        P[mini] = true;
    }

    real sumD = 0;
    for(auto& a : v)
        sumD += a.d;

    std::default_random_engine generator;
    std::uniform_int_distribution<int> dist(0, v.size()-1);
    for(int i = 0; i < 10000; i++)
    {
        int a = dist(generator), b = dist(generator);
        real newda = (v[b].unit->getPosition() - points[v[a].point]).length();
        real newdb = (v[a].unit->getPosition() - points[v[b].point]).length();
        if(auto newd = sumD - (v[a].d + v[b].d) + newda + newdb; newd < sumD)
        {
            sumD = newd;
            std::swap(v[a].unit, v[b].unit);
            v[a].d = newda;
            v[b].d = newdb;
        }
    }
    for(auto assignment : v)
        addUnitPathfindingRequest(assignment.unit, points[assignment.point]);
}

void UserInterface::selectUnit(const Ray& ray, const std::vector<Unit*>& units, bool pre)
{
    auto time = glfwGetTime();
    auto camera = scene->getCamera();

    if(!pre)
    {
        for(auto u : units)
        {
            if(/*!u->isEnemy() && */u->intersectBoundingBox(ray))
            {
                if(time - timeClickedUnit < 0.5 && u == lastClickedUnit)
                {
                    for(auto& unit : units)
                    {
                        if(unit->isEnemy() == u->isEnemy() && isInFrustum(unit))
                            unit->setSelected(true);
                    }
                }
                timeClickedUnit = time;
                lastClickedUnit = u;
                u->setSelected(true);
            }
        }
    }
    else
    {
        for(auto u : units)
        {
            if(/*!u->isEnemy() && */u->intersectBoundingBox(ray))
                u->setPreSelected(true);
            else
                u->setPreSelected(false); 
        }
    }
}

bool UserInterface::handleInput(const Input& input, const std::vector<Unit*>& units)
{
    auto& inputQueue = InputManager::getInstance();

    auto time = glfwGetTime();

    if(input.stateStart == InputType::KeyPress && input.key == GLFW_KEY_GRAVE_ACCENT)
    {
        showConsole = !showConsole;
    }
    
    if(showConsole)
    {
        console->handleInput(input);
        return true;
    }

    if(auto pMovingUnit = scene->getEntity(movingUnit); pMovingUnit)
    {
        auto [x, y] = mouseCoordToScreenCoord(xres, yres, mouseX, mouseY);

        auto pos = scene->getTerrain()->intersect(scene->getCamera()->getViewRay(x, y));
        pMovingUnit->setGeoPosition(pos.to2());
    }

    if(cameraControl->getMode() == Freelook)
        return false;

    if(input.key == GLFW_KEY_LEFT_SHIFT)
    {
        selectingAdditional = (input.stateStart == InputType::KeyPress || input.stateStart == InputType::KeyHold);
        if(input.stateEnd == InputType::KeyRelease)
            selectingAdditional = false;
    }

    if(input.stateStart == InputType::MousePress && input.key == GLFW_MOUSE_BUTTON_1 && inputQueue.isKeyHeld(GLFW_KEY_LEFT_CONTROL))
    {
        auto [x, y] = mouseCoordToScreenCoord(xres, yres, mouseX, mouseY);
        auto e = dynamic_cast<Entity*>(getUnit(scene->getCamera()->getViewRay(x, y), units));
        if(e)
            movingUnit = e->getId();
    }

    if(input.stateStart == InputType::MousePress && input.key == GLFW_MOUSE_BUTTON_1 && !inputQueue.isKeyHeld(GLFW_KEY_LEFT_CONTROL))
    {
        selectState = Clicking;
        drawBoxc1 = mouseCoordToScreenCoord(xres, yres, mouseX, mouseY);
        drawBoxc2 = drawBoxc1;
        setCursor(GLFW_CROSSHAIR_CURSOR);
    }
    if(selectState == DrawingBox)
    {
        for(auto& entity : units)
            entity->setPreSelected(false);
        if(selectState == DrawingBox)
            selectUnits(units, true);
    }
    else if(selectState == NotSelecting)
    {
        auto [x, y] = mouseCoordToScreenCoord(xres, yres, mouseX, mouseY);
        selectUnit(scene->getCamera()->getViewRay(x, y), units, true);
    }
    if(input.stateEnd == InputType::MouseRelease && input.key == GLFW_MOUSE_BUTTON_1)
    {
        for(auto& unit : units)
        {
            if(/*!unit->isEnemy()*/true)
            {
                unit->setPreSelected(false);
                if(!selectingAdditional)
                {
                    unit->setSelected(false);
                }
            }
        }

        if(selectState == DrawingBox)
            selectUnits(units, false);

        else if(selectState == Clicking)
        {
            auto c1 = drawBoxc1;
            selectUnit(scene->getCamera()->getViewRay(c1.x, c1.y), units, false);
        }
        selectState = NotSelecting;
        setCursor(GLFW_ARROW_CURSOR);

        movingUnit = 0;
    }
    if(input.stateStart == InputType::MousePosition)
    {
        mouseX = input.posX;
        mouseY = input.posY;

        drawBoxc2 = mouseCoordToScreenCoord(xres, yres, mouseX, mouseY);

        if(selectState == Clicking && (drawBoxc1-drawBoxc2).length() > 0.02)
            selectState = DrawingBox;
    }
    else if(input.stateEnd == InputType::MouseRelease && input.key == GLFW_MOUSE_BUTTON_2)
    {
        if(drawTarget.size() < 3)
        {
            bool found = false;
            auto [x, y] = mouseCoordToScreenCoord(xres, yres, mouseX, mouseY);
            auto ray = scene->getCamera()->getViewRay(x, y);
            for(auto u : units)
            {
                // TODO: this is not the closest enemy necessarily
                if(u->intersectBoundingBox(ray) && u->isEnemy())
                {
                    for(auto b : units)
                    {
                        if(b->isSelected())
                        {
                            found = true;
                            b->setEnemyTargetId(u->getId());
                        }
                    }
                }
            }

            if(!found)
            {
                auto [px, py] = mouseCoordToScreenCoord(xres, yres, mouseX, mouseY);

                auto pos = scene->getTerrain()->intersect(scene->getCamera()->getViewRay(px, py));
                for(auto unit : units)
                {
                    if(unit->isSelected())
                    {
                        addUnitPathfindingRequest(unit, pos);
                    }
                }
            }
        }
    }

    else if((input.stateStart == InputType::MousePress || input.stateStart == InputType::MouseHold) && input.key == GLFW_MOUSE_BUTTON_2)
    {
        bool anySelected = std::ranges::any_of(units, [](auto unit) { return unit->isSelected(); });
        if(anySelected)
        {
            auto [x, y] = mouseCoordToScreenCoord(xres, yres, mouseX, mouseY);
            auto pos = scene->getTerrain()->intersect(scene->getCamera()->getViewRay(x, y));
            if(drawTarget.empty() || (drawTarget.back() - pos).length() > 0.1)
                drawTarget.push_back(pos);
        }
    }

    if((input.stateEnd == InputType::MouseRelease || input.stateStart == InputType::MouseRelease) && input.key == GLFW_MOUSE_BUTTON_2)
    {
        if(drawTarget.size() > 1)
        {
            // Calculate the points where we are to place destination points
            std::vector<Unit*> selectedUnits;
            for(auto unit : units)
                if(unit->isSelected())
                    selectedUnits.push_back(unit);

            if(!selectedUnits.empty())
                moveDrawnUnits(selectedUnits);
        }

        drawTarget.clear();
    }

    if(input.stateEnd == InputType::KeyRelease && input.key == GLFW_KEY_K)
    {
        for(auto unit : units)
        {
            if(unit->isSelected())
            {
                ((Tank*)unit)->shoot();
            }
        }
    }
    return false;
}


void UserInterface::setResolution(int xres, int yres)
{
    this->xres = xres;
    this->yres = yres;
}


void UserInterface::draw()
{
    GLint depthFunc;
    glGetIntegerv(GL_DEPTH_FUNC, &depthFunc);

    glDepthFunc(GL_LEQUAL);

	glDepthRange(0, 0);
    if(selectState == DrawingBox)
    {
        Line2d line({
            { drawBoxc1.x, drawBoxc1.y, },
            { drawBoxc2.x, drawBoxc1.y, },
            { drawBoxc2.x, drawBoxc2.y, },
            { drawBoxc1.x, drawBoxc2.y, },
            { drawBoxc1.x, drawBoxc1.y, }
                    });
        line.setColor(Vector3(0.2, 0.7, 0.1));
        line.init(scene);
        line.draw();
    }
    if(!drawTarget.empty())
    {
        Line3d line(drawTarget);
        line.setColor(Vector3(0.2, 0.7, 0.1));
        line.setInFront(true);
        line.init(scene);
        line.draw();
    }
    if(showConsole)
    {
        console->draw();
    }
    glDepthFunc(depthFunc);
}

void UserInterface::setCursor(int shape)
{
    if(cursor)
        glfwDestroyCursor(cursor);
    cursor = glfwCreateStandardCursor(shape);
    glfwSetCursor(window, cursor);
}