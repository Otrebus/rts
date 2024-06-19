#pragma once

#include "Camera.h"
#include "Material.h"
#include "Mesh.h"
#include "Mesh3d.h"
#include "Scene.h"
#include "ShaderProgram.h"
#include "Tank.h"
#include "Vector3.h"
#include "Vertex3d.h"
#include <vector>


class Scene;


class SelectionMarkerMesh : public Mesh3d
{
public:
    SelectionMarkerMesh(Entity* tank);

    void draw();
    void update();

    void setSelectionType(bool pre);

    std::pair<std::vector<Vertex3d>, std::vector<int>> calcVertices(Scene* scene);
    void init();

    int pass;
    Entity* tank;
    bool pre;
};
