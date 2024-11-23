#include "Building.h"
#include "Scene.h"
#include "ShapeDrawer.h"


Building::Building(Vector3 pos, int length, int width, std::vector<int> footprint) : Entity(pos, { 0, 1, 0 }, { 0, 0, 1 }), length(length), width(width), footprint(footprint)
{
}

Building::~Building()
{
}

void Building::draw(Material* mat = nullptr)
{
    /*real height = 2.0;
    ShapeDrawer::setInFront(false);
    ShapeDrawer::drawBox(Vector3(int(pos.x), pos.y, pos.z) + Vector3(real(length)/2, real(width)/2, real(height)/2), Vector3(1, 0, 0), length, width, height, Vector3(0.7, 0.7, 0.7));*/
}


void Building::update(real dt)
{
    geoPos = pos.to2();
}


bool Building::buildingWithin(int posX, int posY, int length, int width)
{
    bool withinX = pos.x < posX + length && pos.x + length > posX;
    bool withinY = pos.y < posY + width && pos.y + width > posY;
    return withinX && withinY;
}


bool Building::pointWithinFootprint(int posX, int posY)
{
    int x = posX - pos.x, y = posY - pos.y;
    return x >= 0 && x <= length && y >= 0 && y <= width && footprint[y*(length+1)+x];
}


bool Building::canBePlaced(int posX, int posY, int length, int width, Scene* scene)
{
    for(auto building : scene->getBuildings())
    {
        if(building->buildingWithin(posX, posY, length, width))
            return false;
    }
    return true;
}
