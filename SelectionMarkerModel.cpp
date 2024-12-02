#include "LambertianMaterial.h"
#include "SelectionDecalMaterial.h"
#include "SelectionMarkerModel.h"
#include "Tank.h"
#include "Terrain.h"


SelectionMarkerModel::SelectionMarkerModel(Entity* tank)
{
    this->tank = tank;
}


void SelectionMarkerModel::init(Scene* scene)
{
    auto decalMesh = new Mesh3d();
    addMesh(*decalMesh);
    decalMesh->setScene(scene);
}

void SelectionMarkerModel::update()
{
    meshes[0]->tearDown();
    meshes[0]->init();
    meshes[0]->setScene(meshes[0]->scene);
}
