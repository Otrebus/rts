#pragma once

#include "Mesh3d.h"
#include "Tank.h"


class Scene;


class SelectionMarkerMesh : public Mesh3d
{
public:
    SelectionMarkerMesh(real width, real length, bool circular);

    void draw(Material* mat = nullptr);
    void update(Vector2 pos);

    void setSelectionType(bool pre);

    std::pair<std::vector<Vertex3>, std::vector<int>> calcVertices(Vector2 pos);
    void init(Vector2 pos);

    int pass;
    bool pre;

    real length, width;
    bool circular;
};
