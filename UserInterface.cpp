#include "UserInterface.h"
#include "Input.h"
#include "Line.h"
#include "Math.h"
#include "Entity.h"
#include "Ray.h"


UserInterface::UserInterface(Scene* scene) : scene(scene)
{
    drawBoxc1 = { -0.5, -0.5 };
    drawBoxc2 = { 0.5, 0.5 };
}


struct Polyhedra {
    std::vector<Vector3> vertices;
    std::vector<std::pair<int, int>> edges;
    std::vector<int> faceIndices;
    std::vector<std::pair<int, int>> faces;
};

bool intersectsSAT(const Polyhedra& a, const Polyhedra& b)
{
    std::vector<Ray> P;
    for(auto& [i, n] : a.faces)
    {
        auto v1 = (a.vertices[a.faceIndices[i]] - a.vertices[a.faceIndices[i+1]]);
        auto v2 = (a.vertices[a.faceIndices[i+2]] - a.vertices[a.faceIndices[i]]);
        P.push_back({ a.vertices[a.faceIndices[i+1]], v1%v2 });
    }
    for(auto& [i, n] : b.faces)
    {
        auto v1 = (b.vertices[b.faceIndices[i]] - b.vertices[b.faceIndices[i+1]]);
        auto v2 = (b.vertices[b.faceIndices[i+2]] - b.vertices[b.faceIndices[i]]);
        P.push_back({ b.vertices[b.faceIndices[i+1]], v1%v2 });
    }

    for(auto [a1, a2] : a.edges)
    {
        for(auto [b1, b2] : b.edges)
        {
            P.push_back({ a.vertices[a1], (a.vertices[a1]-a.vertices[a2])%(b.vertices[b1]-b.vertices[b2]) });
        }
    }


}

bool intersectsFrustum(Vector3 pos, Vector3 v[4], Entity& entity)
{
    auto M = getDirectionMatrix(entity.dir, entity.up);

    auto [c1, c2] = entity.bbox;

    Vector3 C[8] = {
        c1,
        Vector3(c1.x, c2.y, c1.z),
        Vector3(c2.x, c2.y, c1.z),
        Vector3(c2.x, c1.y, c1.z),
        Vector3(c1.x, c2.y, c2.z),
        Vector3(c2.x, c2.y, c2.z),
        c2,
        Vector3(c1.x, c2.y, c2.z),
    };
    
    for(auto& c : C)
        c = M*c + entity.pos;

    int ci[6][4] = {
        { 0, 1, 2, 3 },
        { 0, 5, 6, 2 },
        { 2, 6, 7, 3 },
        { 3, 7, 4, 0 },
        { 0, 1, 5, 4 },
        { 7, 6, 5, 4 }
    };

    Ray S[16];
    for(int i = 0; i < 6; i++)
        S[i] = { C[ci[i][0]], ((C[ci[i][0]]-C[ci[i][1]])%((C[ci[i][2]]-C[ci[i][1]])).normalized()) };
    for(int i = 0; i < 6; i++)
        S[i] = { C[ci[i][0]], ((C[ci[i][0]]-C[ci[i][1]])%((C[ci[i][2]]-C[ci[i][1]])).normalized()) };

    //Vector3 E[12] = {
    //    C[1]-C[0],
    //    C[2]-C[1],
    //    C[3]-C[2],
    //    C[0]-C[3],
    //    C[4]-C[0],
    //    C[5]-C[1],
    //    C[6]-C[2],
    //    C[7]-C[3],
    //    C[5]-C[4],
    //    C[6]-C[5],
    //    C[7]-C[6],
    //    C[4]-C[7]
    //};

    //Ray S[16] = {
    //    { pos, (v[0]%v[1]).normalized() },
    //    { pos, (v[1]%v[2]).normalized() },
    //    { pos, (v[2]%v[3]).normalized() },
    //    { pos, (v[3]%v[0]).normalized() },
    //    { C[0], (E[3]%E[0]).normalized() },
    //    { C[0], (E[4]%E[0]).normalized() },
    //    { C[1], (E[5]%E[1]).normalized() },
    //    { C[2], (E[6]%E[2]).normalized() },
    //    { C[3], (E[7]%E[3]).normalized() },
    //    { C[0], (E[4]%E[0]).normalized() },
    //};

    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {

        }
    }
}


void UserInterface::selectEntities(std::vector<Entity*> entities)
{
    auto camera = scene->getCamera();

    auto c1 = drawBoxc1, c2 = drawBoxc2;

    auto tr = Vector2(std::max(c1.x, c2.x), std::max(c1.y, c2.y));
    auto tl = Vector2(std::min(c1.x, c2.x), std::max(c1.y, c2.y));
    auto bl = Vector2(std::min(c1.x, c2.x), std::min(c1.y, c2.y));
    auto br = Vector2(std::max(c1.x, c2.x), std::min(c1.y, c2.y));

    Vector3 v[4] = {
        getViewRay(*camera, tr.x, tr.y),
        getViewRay(*camera, tl.x, tl.y),
        getViewRay(*camera, bl.x, bl.y),
        getViewRay(*camera, br.x, br.y)
    };

    for(auto e : entities)
    {
        if(intersectsFrustum(camera->pos, v, *e))
            e->setSelected(true); // TODO: clean
        else
            e->setSelected(false);
    }
}


void UserInterface::handleInput(const Input& input)
{
    auto& inputQueue = *input.inputQueue;

    if(input.stateStart == InputType::MousePress)
    {
        drawingBox = true;
        drawBoxc1.x = real(2*mouseX)/xres - 1;
        drawBoxc1.y = -(real(2*mouseY)/yres - 1);
    }
    if(input.stateEnd == InputType::MouseRelease)
    {
        drawingBox = false;
    }
    if(input.stateStart == InputType::MousePosition)
    {
        mouseX = input.posX;
        mouseY = input.posY;

        drawBoxc2.x = real(2*mouseX)/xres - 1;
        drawBoxc2.y = -(real(2*mouseY)/yres - 1);
    }
}


void UserInterface::setResolution(int xres, int yres)
{
    this->xres = xres;
    this->yres = yres;
}


void UserInterface::draw()
{
    Line2d line({
        { drawBoxc1.x, drawBoxc1.y, },
        { drawBoxc2.x, drawBoxc1.y, },
        { drawBoxc2.x, drawBoxc2.y, },
        { drawBoxc1.x, drawBoxc2.y, },
        { drawBoxc1.x, drawBoxc1.y, }
    });
    line.setUp(scene);

    if(drawingBox)
    {
        line.draw();
    }
}