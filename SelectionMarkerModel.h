#pragma once
#include "BoundingBox.h"
#include "Camera.h"
#include "Material.h"
#include "Matrix4.h"
#include "Mesh.h"
#include "Mesh3d.h"
#include "Model3d.h"
#include "ShaderProgram.h"
#include "Vector2.h"
#include "Vector3.h"
#include <map>


class Tank;


class SelectionMarkerModel : public Model3d
{

public:
    SelectionMarkerModel(Entity* tank);
    ~SelectionMarkerModel();

    std::pair<std::vector<Vertex3>, std::vector<int>> calcVertices(Scene* scene);

    void update();

    void init(Scene* scene);
private:
    Entity* tank;
};
