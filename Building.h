#pragma once
#pragma once

#include "BoundingBox.h"
#include "Utils.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Entity.h"

class Building : public Entity
{
public:
    Building(Vector3 pos, int length, int width);
    virtual ~Building();

    void draw(Material* mat);
    void update(real dt);

    int length, width;
};
