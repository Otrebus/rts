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

    bool within(int posX, int posY, int length, int width);
    void draw(Material* mat);
    void update(real dt);
    static bool isAdmissible(int x, int y, int length, int width, Scene* scene);

    int length, width;
};
