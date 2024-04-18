#pragma once
#include "Mesh3d.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Matrix4.h"
#include "ShaderProgram.h"
#include "Material.h"
#include "Camera.h"
#include "Mesh.h"
#include <map>
#include "Model3d.h"
#include "BoundingBox.h"


class Tank;


class SelectionMarkerModel : public Model3d
{

public:
    SelectionMarkerModel(Entity* tank);
    ~SelectionMarkerModel();

    std::pair<std::vector<Vertex3d>, std::vector<int>> calcVertices(Scene* scene);

    void update();

    void init(Scene* scene);
private:
    Entity* tank;
};
