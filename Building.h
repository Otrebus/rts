#pragma once
#pragma once

#include "BoundingBox.h"
#include "Utils.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Unit.h"


class Scene;


class Building : public Unit
{
public:
    Building(int x, int y, int length, int width, std::vector<int> footprint);
    virtual ~Building();

    bool buildingWithin(real posX, real posY, int length, int width);
    bool pointWithinFootprint(int posX, int posY);
    void draw(Material* mat);
    void update(real dt);
    void plant(const Terrain& terrain);
    void init(Scene& scene);
    bool canBePlaced(real x, real y, int length, int width, Scene* scene);

    real getAverageElevation(const Terrain& terrain);

    Entity* spawnWreck();

    std::vector<int> footprint;

    int length, width;
};
