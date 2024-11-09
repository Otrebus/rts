#pragma once
#include "Model3d.h"
#include <map>

class Tank;

class BuildingPlacerModel : public Model3d
{

public:
    BuildingPlacerModel(Entity* tank);

    std::pair<std::vector<Vertex3>, std::vector<int>> calcVertices(Scene* scene);

    void update();

    void init(Scene* scene);
private:
    Entity* tank;
};
