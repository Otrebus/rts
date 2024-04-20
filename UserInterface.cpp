#define NOMINMAX
#include "UserInterface.h"
#include "Input.h"
#include "Line.h"
#include "Math.h"
#include "Entity.h"
#include "Ray.h"
#include "PathFinding.h"


UserInterface::UserInterface(GLFWwindow* window, Scene* scene, CameraControl* cameraControl) : scene(scene), cameraControl(cameraControl), cursor(nullptr), window(window)
{
    drawBoxc1 = { 0, 0 };
    drawBoxc2 = { 0, 0 };
    selectState = NotSelecting;
    intersecting = false;
}


struct Polyhedra {
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
        else {
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
    for (auto& [i, n] : a.faces)
    {
        auto v1 = (v[f[i]] - v[f[i+1]]).normalized();
        auto v2 = (v[f[i+2]] - v[f[i]]).normalized();
        if (auto k = v1%v2; k && testSides(a, b, { v[f[i+1]], (v1%v2).normalized() }))
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
            auto u = (a.vertices[a1]-a.vertices[a2]).normalized();
            auto v = (b.vertices[b1]-b.vertices[b2]).normalized();
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


void UserInterface::selectEntities(std::vector<Entity*> entities)
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

    for(auto e : entities)
    {
        if(intersectsFrustum(camera->getPos(), v, *e, scene))
            e->setSelected(true); // TODO: clean
        else
            e->setSelected(false);
    }
    std::cout << (glfwGetTime() - time) << std::endl;
}

Entity* UserInterface::getEntity(const Ray& ray, const std::vector<Entity*>& entities) const
{
    for(auto e : entities)
        if(e->intersectBoundingBox(ray))
            return e;
    return nullptr;
}

void UserInterface::selectEntity(const Ray& ray, const std::vector<Entity*>& entities)
{
    auto time = glfwGetTime();
    auto camera = scene->getCamera();
    
    for(auto e : entities)
    {
        if(e->intersectBoundingBox(ray))
            e->setSelected(true);
        else
            e->setSelected(false);
    }
    std::cout << (glfwGetTime() - time) << std::endl;
}

void UserInterface::handleInput(const Input& input, const std::vector<Entity*>& entities)
{
    auto& inputQueue = InputQueue::getInstance();

    if(movingEntity)
    {
        auto [x, y] = mouseCoordToScreenCoord(xres, yres, mouseX, mouseY);

        auto pos = scene->getTerrain()->intersect(scene->getCamera()->getViewRay(x, y));
        movingEntity->setGeoPosition(pos.to2());
    }

    if(cameraControl->getMode() == Freelook)
        return;

    if(input.stateStart == InputType::MousePress && input.key == GLFW_MOUSE_BUTTON_1 && inputQueue.isKeyHeld(GLFW_KEY_LEFT_CONTROL))
    {
        auto [x, y] = mouseCoordToScreenCoord(xres, yres, mouseX, mouseY);
        auto e = getEntity(scene->getCamera()->getViewRay(x, y), entities);
        if(e)
            movingEntity = e;
    }

    if(input.stateStart == InputType::MousePress && input.key == GLFW_MOUSE_BUTTON_1 && !inputQueue.isKeyHeld(GLFW_KEY_LEFT_CONTROL))
    {
        selectState = Clicking;
        drawBoxc1 = mouseCoordToScreenCoord(xres, yres, mouseX, mouseY);
        drawBoxc2 = drawBoxc1;
        setCursor(GLFW_CROSSHAIR_CURSOR);
    }
    if(input.stateEnd == InputType::MouseRelease && input.key == GLFW_MOUSE_BUTTON_1)
    {
        if(selectState == DrawingBox)
            selectEntities(entities);
        else if(selectState == Clicking)
        {
            auto c1 = drawBoxc1;
            selectEntity(scene->getCamera()->getViewRay(c1.x, c1.y), entities);
        }
        selectState = NotSelecting;
        setCursor(GLFW_ARROW_CURSOR);

        movingEntity = nullptr;
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
        auto [px, py] = mouseCoordToScreenCoord(xres, yres, mouseX, mouseY);

        auto pos = scene->getTerrain()->intersect(scene->getCamera()->getViewRay(px, py));
        for(auto entity : entities)
        {
            if(entity->selected)
            {
                PathFindingRequest* request = new PathFindingRequest;
                request->requester = entity;
                request->start = entity->getPosition().to2();
                request->dest = pos.to2();
                addPathFindingRequest(request);
                entity->setCurrentPathfindingRequest(request);
            }
        }
    }
}


void UserInterface::setResolution(int xres, int yres)
{
    this->xres = xres;
    this->yres = yres;
}


void UserInterface::draw()
{
    if(selectState == DrawingBox) {
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
}

void UserInterface::setCursor(int shape)
{
    if(cursor)
        glfwDestroyCursor(cursor);
    cursor = glfwCreateStandardCursor(shape);
    glfwSetCursor(window, cursor);
}