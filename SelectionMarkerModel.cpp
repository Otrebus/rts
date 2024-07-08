#include "LambertianMaterial.h"
#include "LineMaterial.h"
#include "LineMesh3d.h"
#include "Math.h"
#include "Scene.h"
#include "SelectionDecalMaterial.h"
#include "SelectionMarkerModel.h"
#include "Tank.h"
#include "Terrain.h"


SelectionMarkerModel::SelectionMarkerModel(Entity* tank)
{
    this->tank = tank;
}


SelectionMarkerModel::~SelectionMarkerModel() // TODO: delete
{
}


std::pair<std::vector<Vertex3>, std::vector<int>> SelectionMarkerModel::calcVertices(Scene* scene)
{
    auto tankPos = tank->getPosition();

    int xc = tankPos.x, yc = tankPos.y;

    std::vector<Vertex3> vs;
    for(int y = yc-1; y < yc+3; y++)
    {
        for(int x = xc-1; x < xc+3; x++)
        {
            auto pos = scene->getTerrain()->getPoint(x, y);
            pos.z += 0.01;
            vs.push_back({ pos.x, pos.y, pos.z, 0, 0, 1, x-real(xc), x-real(yc) });
        }
    }

    std::vector<int> triangles;

    for(int y = 0; y < 3; y++)
    {
        for(int x = 0; x < 3; x++)
        {
            int i1 = y*4+x, i2 = y*4+x+1, i3 = (y+1)*4+x+1, i4 = (y+1)*4 + x;
            triangles.insert(triangles.end(), { i1, i2, i3, i1, i3, i4 });
        }
    }
    return { vs, triangles };
}


void SelectionMarkerModel::init(Scene* scene)
{
    auto [vs, triangles] = calcVertices(scene);

    auto material = new LambertianMaterial({ 0, 0.8, 0.1 });

    Mesh3d* decalMesh;

    decalMesh = new Mesh3d(vs, triangles, material);
    addMesh(*decalMesh);
    decalMesh->setScene(scene);
}

void SelectionMarkerModel::update()
{
    auto [vs, triangles] = calcVertices(meshes[0]->scene);
    meshes[0]->v = vs;
    meshes[0]->tearDown();
    meshes[0]->setScene(meshes[0]->scene);
}
