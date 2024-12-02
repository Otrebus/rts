#pragma once

#include "Mesh3d.h"
#include "Tank.h"


class Scene;


class SelectionMarkerMesh : public Mesh3d
{
public:
    SelectionMarkerMesh(Entity* entity, int width, int length);

    void draw(Material* mat = nullptr);
    void update();

    void setSelectionType(bool pre);

    std::pair<std::vector<Vertex3>, std::vector<int>> calcVertices(Scene* scene);
    void init();

    int pass;
    Entity* entity;
    bool pre;

    int length, width;
    bool circular;
};
