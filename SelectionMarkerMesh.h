#pragma once

#include <vector>
#include "Vertex3d.h"
#include "Material.h"
#include "ShaderProgram.h"
#include "Vector3.h"
#include "Camera.h"
#include "Scene.h"
#include "Mesh.h"
#include "Mesh3d.h"
#include "Tank.h"


class scene;


class SelectionMarkerMesh : public Mesh3d
{
public:
    SelectionMarkerMesh(Entity* tank);
    SelectionMarkerMesh(std::vector<Vertex3d> vertices, std::vector<int> triangles, Material* material);

    void draw();
    void update();
    void setSelectionType(bool pre);

    std::pair<std::vector<Vertex3d>, std::vector<int>> calcVertices(Scene* scene);
    void init(Scene* scene);

    int pass;
    Entity* tank;
    Scene* scene;
    bool pre;
};
