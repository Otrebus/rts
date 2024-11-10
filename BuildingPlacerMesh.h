#pragma once
#include "Model3d.h"
#include <map>


class BuildingPlacerMesh : public Mesh3d
{
public:
    BuildingPlacerMesh(Scene* scene, int width, int height);

    std::pair<std::vector<Vertex3>, std::vector<int>> calcVertices(Scene* scene, int xpos, int ypos);

    void update(int xpos, int ypos);

    void init();
private:
    int width, height;
};
