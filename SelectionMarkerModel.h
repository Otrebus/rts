#pragma once
#include "Model3d.h"
#include <map>

class Tank;

class SelectionMarkerModel : public Model3d
{

public:
    SelectionMarkerModel(Entity* tank);

    void update();

    void init(Scene* scene);
private:
    Entity* tank;
};
