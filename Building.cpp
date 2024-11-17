#include "Building.h"
#include "Scene.h"
#include "ShapeDrawer.h"


Building::Building(Vector3 pos, int length, int width) : Entity(pos, { 0, 1, 0 }, { 0, 0, 1 }), length(length), width(width)
{
}

Building::~Building()
{
}

void Building::draw(Material* mat = nullptr)
{
    real height = 2.0;
    ShapeDrawer::setInFront(false);
    ShapeDrawer::drawBox(Vector3(int(pos.x), pos.y, pos.z) + Vector3(real(length)/2, real(width)/2, real(height)/2), Vector3(1, 0, 0), length, width, height);
}


void Building::update(real dt)
{
    geoPos = pos.to2();
}


bool Building::isAdmissible(int posX, int posY, int length, int width, Scene* scene)
{
    for(auto building : scene->getBuildings())
    {
        bool withinX = building->pos.x < posX + length && building->pos.x + building->length > posX;
        bool withinY = building->pos.y < posY + width && building->pos.y + building->width > posY;
        if(withinX && withinY)
            return false;
    }
    return true;
}
