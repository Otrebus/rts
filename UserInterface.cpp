#define NOMINMAX
#include "Entity.h"
#include "InputManager.h"
#include "Line.h"
#include "Vehicle.h"
#include "Math.h"
#include "Truck.h"
#include "PathFinding.h"
#include "Ray.h"
#include "Tank.h"
#include "Unit.h"
#include "UserInterface.h"
#include "Building.h"
#include <numeric>
#include <random>
#include <ranges>
#include <format>
#include "LambertianMaterial.h"
#include "BuildingPlacerMesh.h"
#include "SelectionMarkerMesh.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include "Font.h"

UserInterface::UserInterface(GLFWwindow* window, Scene* scene, CameraControl* cameraControl) : scene(scene), cameraControl(cameraControl), cursor(nullptr), window(window), lastClickedUnit(nullptr)
{
    drawBoxc1 = { 0, 0 };
    drawBoxc2 = { 0, 0 };
    selectState = NotSelecting;
    buildingPlacingState = NotPlacingBuilding;
    buildingPlacerMesh = nullptr;

    selectionMesh = nullptr;
    intersecting = false;
    selectingAdditional = false;
    showConsole = false;
    console = new Console(scene);
    console->init();
    font = new Font(*scene, "Roboto-Bold.ttf");
    fps = -1;
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


void addUnitPathfindingRequest(Unit* unit, Vector2 pos)
{
    PathFindingRequest* request = new PathFindingRequest;
    request->requester = unit;
    request->start = unit->getPosition().to2();
    request->dest = pos;
    addPathFindingRequest(request);
    unit->setCurrentPathfindingRequest(request);
}


void UserInterface::selectUnits(std::vector<Unit*> units, bool pre)
{
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
    std::uniform_int_distribution<int> dist(0, int(v.size()-1));
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
    {
        if(!selectingAdditional)
            assignment.unit->commandQueue.clear();
        assignment.unit->commandQueue.push(MoveCommand(points[assignment.point].to2()));
    }

}

void UserInterface::selectUnit(const Ray& ray, const std::vector<Unit*>& units, bool pre)
{
    auto time = real(glfwGetTime());
    auto camera = scene->getCamera();

    if(!pre)
    {
        for(auto u : units)
        {
            if(/*!u->isEnemy() && */u->intersectBoundingBox(ray))
            {
                if(time - timeClickedUnit < 0.5 && u == lastClickedUnit)
                    for(auto& unit : units)
                        if(unit->isEnemy() == u->isEnemy() && isInFrustum(unit) && typeid(*u) == typeid(*unit))
                            unit->setSelected(true);
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
        console->setOpen(showConsole);
        return true;
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
    {
        // TODO: is there some better way of resetting everything when going into freelook?
        selectingAdditional = false;
        selectState = NotSelecting;
        setCursor(GLFW_ARROW_CURSOR);
        return false;
    }

    if(input.key == GLFW_KEY_B && input.stateStart == InputType::KeyPress)
    {
        std::vector<Unit*> selectedVehicles;
        for(auto unit : units)
        {
            if(unit->isSelected() && dynamic_cast<Truck*>(unit))
                selectedVehicles.push_back(unit);
        }
        if(!selectedVehicles.empty())
        {
            buildingPlacingState = buildingPlacingState == PlacingBuilding ? NotPlacingBuilding : PlacingBuilding;
            if(buildingPlacingState == PlacingBuilding)
            {
                if(!buildingPlacerMesh)
                {
                    buildingPlacerMesh = new BuildingPlacerMesh(scene, 3, 4);
                    buildingPlacerMesh->init();
                }
            }
            else
            {
                delete buildingPlacerMesh;
                buildingPlacerMesh = nullptr;
            }
        }
        else
        {
            if(buildingPlacingState == PlacingBuilding)
                buildingPlacingState = NotPlacingBuilding;
        }
    }

    if(buildingPlacerMesh)
    {
        auto [px, py] = mouseCoordToScreenCoord(xres, yres, mouseX, mouseY);

        auto pos = scene->getTerrain()->intersect(scene->getCamera()->getViewRay(px, py));

        std::vector<int> footprint = {
            1, 0, 0, 1,
            1, 0, 0, 1,
            1, 0, 0, 1,
            1, 1, 1, 1,
            1, 1, 1, 1
        };
        auto building = new Building(int(pos.x), int(pos.y), 3, 4, footprint);
        buildingPlacerMesh->update(int(pos.x), int(pos.y), building->canBePlaced(int(pos.x) + 3.0f/2, int(pos.y) + 4.0f/2, 3, 4, scene));
        delete building;
    }

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
        if(buildingPlacingState == PlacingBuilding)
        {
            std::vector<Unit*> selectedVehicles;
            for(auto unit : units)
            {
                if(unit->isSelected() && dynamic_cast<Truck*>(unit))
                    selectedVehicles.push_back(unit);
            }

            auto [px, py] = mouseCoordToScreenCoord(xres, yres, mouseX, mouseY);
            auto pos = scene->getTerrain()->intersect(scene->getCamera()->getViewRay(px, py));
            std::vector<int> footprint = {
                1, 0, 0, 1,
                1, 0, 0, 1,
                1, 0, 0, 1,
                1, 1, 1, 1,
                1, 1, 1, 1
            };
            auto building = Building(int(pos.x), int(pos.y), 3, 4, footprint);
            if(building.canBePlaced(int(pos.x) + 3.0f/2, int(pos.y) + 4.0f/2, 3, 4, scene))
            {
                if(!selectedVehicles.empty())
                {
                    for(auto vehicle : selectedVehicles)
                    {
                        if(!selectingAdditional)
                            vehicle->commandQueue.clear();
                        vehicle->commandQueue.push(BuildCommand(Vector2(std::floor(pos.x), std::floor(pos.y)), 3, 4));
                    }
                }
            }
            else
            {
                for(auto building : scene->getBuildings())
                {
                    if(building->buildingWithin(int(pos.x) + 3.0f/2, int(pos.y) + 4.0f/2, 3, 4))
                    {
                        scene->removeEntity(building);
                    }
                }
            }
        }
        else if(selectState == PickingCircleCenter)
        {
            selectState = DrawingCircle;
            auto [px, py] = mouseCoordToScreenCoord(xres, yres, mouseX, mouseY);
            auto pos = scene->getTerrain()->intersect(scene->getCamera()->getViewRay(px, py));
            // TODO: check for being outside terrain
            circleCenter = pos.to2();

            selectionMesh = new SelectionMarkerMesh(0.f, 0.f, true);
            selectionMesh->setScene(scene);
            selectionMesh->init(circleCenter);
        }
        else
        {
            selectState = Clicking;
            drawBoxc1 = mouseCoordToScreenCoord(xres, yres, mouseX, mouseY);
            drawBoxc2 = drawBoxc1;
            setCursor(GLFW_CROSSHAIR_CURSOR);
        }
    }
    if(selectState == DrawingCircle)
    {
        auto [px, py] = mouseCoordToScreenCoord(xres, yres, mouseX, mouseY);
        auto pos = scene->getTerrain()->intersect(scene->getCamera()->getViewRay(px, py));
        selectionMesh->setRadius((pos.to2() - circleCenter).length());
        selectionMesh->init(circleCenter);
        selectionMesh->update(circleCenter);
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
        if(buildingPlacingState != PlacingBuilding && selectState != DrawingCircle)
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
        }

        if(selectState == DrawingBox)
            selectUnits(units, false);

        if(selectState == DrawingCircle)
        {
            delete selectionMesh;
            selectionMesh = nullptr;
            selectState = NotSelecting;
            for(auto& unit : units)
            {
                auto [px, py] = mouseCoordToScreenCoord(xres, yres, mouseX, mouseY);
                auto pos = scene->getTerrain()->intersect(scene->getCamera()->getViewRay(px, py));
                auto circleRadius = (pos.to2() - circleCenter).length();
                if(unit->isSelected())
                {
                    if(!selectingAdditional)
                        unit->commandQueue.clear();
                    unit->commandQueue.push(ExtractCommand(circleCenter, circleRadius));
                }
            }
        }

        else if(selectState == Clicking)
        {
            auto c1 = drawBoxc1;
            selectUnit(scene->getCamera()->getViewRay(c1.x, c1.y), units, false);
        }
        selectState = NotSelecting;
        setCursor(GLFW_ARROW_CURSOR);

        movingUnit = 0;
    }

    if(input.stateStart == InputType::KeyPress && input.key == GLFW_KEY_B)
    {
        for(auto unit : units)
        {
            if(unit->isSelected() && dynamic_cast<Building*>(unit))
            {
                auto factory = (Building*) unit;
                bool isWithin = false;
                for(auto unit : units)
                {
                    if(unit != factory && (unit->getGeoPosition() - factory->getGeoPosition()).length() < 1.f)
                    {
                        isWithin = true;
                        break;
                    }
                }
                if(!isWithin)
                    factory->produceTank();
            }
        }
    }
    if(input.stateStart == InputType::KeyPress && input.key == GLFW_KEY_H)
    {
        selectState = PickingCircleCenter;
    }

    if(input.stateStart == InputType::MousePosition)
    {
        mouseX = int(input.posX);
        mouseY = int(input.posY);

        drawBoxc2 = mouseCoordToScreenCoord(xres, yres, mouseX, mouseY);

        if(selectState == Clicking && (drawBoxc1-drawBoxc2).length() > 0.02 && buildingPlacingState != PlacingBuilding)
            selectState = DrawingBox;

        if(showCoordinates.varInt())
        {
            auto coords = drawBoxc2;
            auto ray = scene->getCamera()->getViewRay(coords.x, coords.y);
            mouseGeoCoords = scene->getTerrain()->intersect(ray).to2();
        }
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
                        if(!selectingAdditional)
                            unit->commandQueue.clear();
                        unit->commandQueue.push(MoveCommand(pos.to2()));
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
            if(drawTarget.empty() || (drawTarget.back() - pos).length() > 0.1f)
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


void UserInterface::update(real dt)
{
    const int avgInterval = 3;
    fps = fps == -1 ? dt : (((avgInterval-1)*fps + 1/dt))/avgInterval;
}


void UserInterface::draw()
{
    glPolygonOffset(-1.0, -1.0);
    for(auto& unit : scene->getUnits())
        unit->drawSelectionDecal(0);
    if(selectionMesh)
    {
        selectionMesh->pre = true;
        selectionMesh->pass = 0;
        selectionMesh->draw();
    }

    for(auto& unit : scene->getUnits())
        unit->drawSelectionDecal(1);

    if(selectionMesh)
    {
        selectionMesh->pass = 1;
        selectionMesh->draw();
    }

    glPolygonOffset(-1.0, -2.0);
    for(auto& unit : scene->getUnits())
        unit->drawSelectionDecal(2);
    if(selectionMesh)
    {
        selectionMesh->pass = 2;
        selectionMesh->draw();
    }

    if(showFps.varInt())
        font->draw(*scene, realToString(fps, 3), { 0.952f, 0.98f }, 0.035f, Vector3(0.2f, 0.9f, 0.2f));

    if(showCoordinates.varInt())
        font->draw(*scene, std::format("({:.2f}, {:.2f})", mouseGeoCoords.x, mouseGeoCoords.y), { 0.852f, -0.95f }, 0.035f, Vector3(0.2f, 0.9f, 0.2f));

    GLint depthFunc;
    glGetIntegerv(GL_DEPTH_FUNC, &depthFunc);

    glDepthFunc(GL_LEQUAL);

	GLfloat depthRange[2];
    glGetFloatv(GL_DEPTH_RANGE, depthRange);

    glDepthRange(0, 0);
    if(buildingPlacingState == PlacingBuilding)
    {
        buildingPlacerMesh->updateUniforms();
        buildingPlacerMesh->draw();
    }
    
    if(selectState == DrawingBox)
    {
        Line2d line({
            { drawBoxc1.x, drawBoxc1.y, },
            { drawBoxc2.x, drawBoxc1.y, },
            { drawBoxc2.x, drawBoxc2.y, },
            { drawBoxc1.x, drawBoxc2.y, },
            { drawBoxc1.x, drawBoxc1.y, }
        });
        line.setColor(Vector4(0.2f, 0.7f, 0.1f, 1.0f));
        line.init(scene);
        line.draw();
    }
    if(!drawTarget.empty())
    {
        Line3d line(drawTarget);
        line.setColor(Vector4(0.2f, 0.7f, 0.1f, 1.0f));
        line.setInFront(true);
        line.init(scene);
        line.draw();
    }
    if(console->isVisible())
    {
        console->draw();
    }
    glDepthFunc(depthFunc);
    glDepthRange(depthRange[0], depthRange[1]);
}

void UserInterface::setCursor(int shape)
{
    if(cursor)
        glfwDestroyCursor(cursor);
    cursor = glfwCreateStandardCursor(shape);
    glfwSetCursor(window, cursor);
}

ConsoleVariable UserInterface::showFps("showFps", 1);
ConsoleVariable UserInterface::showCoordinates("showCoordinates", 1);
ConsoleVariable UserInterface::drawCommands("drawCommands", 1);
