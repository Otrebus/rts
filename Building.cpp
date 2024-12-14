#pragma once

#include "Building.h"
#include "Scene.h"
#include "ShapeDrawer.h"
#include "BoundingBox.h"
#include "SelectionMarkerMesh.h"
#include "Terrain.h"


Building::Building(int x, int y, int length, int width, std::vector<int> footprint) : Unit(pos, { 1, 0, 0 }, { 0, 0, 1 }), length(length), width(width), footprint(footprint)
{
    real height = 2.0;
    geoDir = dir.to2();
    pos = Vector3(x+real(length)/2, y+real(width)/2, 0);
    geoPos = pos.to2();
    boundingBox = BoundingBox(Vector3(-real(length)/2, -real(width)/2, -real(height)/2), Vector3(real(length)/2, real(width)/2, real(height)/2));

    selectionMarkerMesh = new SelectionMarkerMesh(length+0.2, width+0.2, false);
}

Building::~Building()
{
}

void Building::draw(Material* mat = nullptr)
{
    real height = 2.0;
    ShapeDrawer::setInFront(false);
    real W = 0.1;
    real L = 1.0;
    //ShapeDrawer::drawBox(Vector3(int(pos.x), pos.y, pos.z) + Vector3(real(length)/2, real(width)/2, real(height)/2), Vector3(1, 0, 0), length, width, height, Vector3(0.7, 0.7, 0.7));

    // TODO: flip L and W
    ShapeDrawer::drawBox(Vector3(pos.x, pos.y, pos.z) + Vector3(-real(W)/2+real(length)/2, 0, -real(height)/6), Vector3(1, 0, 0), W, width, height*2.0/3, Vector3(0.7, 0.7, 0.7));
    ShapeDrawer::drawBox(Vector3(pos.x, pos.y, pos.z) + Vector3(real(W)/2-real(length)/2, 0, -real(height)/6), Vector3(1, 0, 0), W, width, height*2.0/3, Vector3(0.7, 0.7, 0.7));

    ShapeDrawer::drawBox(Vector3(pos.x, pos.y, pos.z) + Vector3(0, real(width/2) - real(L)/2, 0), Vector3(1, 0, 0), length-W*2, L, height, Vector3(0.7, 0.7, 0.7));
    //ShapeDrawer::drawBox(Vector3(real(pos.x), real(pos.y), real(pos.z)), Vector3(1, 0, 0), length, width, height, Vector3(0.7, 0.7, 0.7));
}

real Building::getAverageElevation(const Terrain& terrain)
{
    // This isn't really the total average but the average of the four corners
    auto x = Vector3(geoDir.x, geoDir.y, 0).normalized();
    auto y = Vector3(-geoDir.y, geoDir.x, 0).normalized();
    auto z = Vector3(0, 0, 1).normalized();

    auto height = (boundingBox.c2.z-boundingBox.c1.z)/2;
    auto length = (boundingBox.c2.x-boundingBox.c1.x)/2;
    auto width = (boundingBox.c2.y-boundingBox.c1.y)/2;

    auto a = geoPos.to3() + x*length + y*width;
    auto b = geoPos.to3() - x*length + y*width;
    auto c = geoPos.to3() - x*length - y*width;
    auto d = geoPos.to3() + x*length - y*width;

    auto ah = terrain.getElevation(a.x, a.y);
    auto bh = terrain.getElevation(b.x, b.y);
    auto ch = terrain.getElevation(c.x, c.y);
    auto dh = terrain.getElevation(d.x, d.y);

    Vector3 A = Vector3(a.x, a.y, ah);
    Vector3 B = Vector3(b.x, b.y, bh);
    Vector3 C = Vector3(c.x, c.y, ch);
    Vector3 D = Vector3(d.x, d.y, dh);

    auto h = ((A+B+C+D)/4.f).z;
    return h;
}

void Building::plant(const Terrain& terrain)
{
    auto h = getAverageElevation(terrain);
    pos = Vector3(geoPos.x, geoPos.y, h);
    setPosition(pos);
    setDirection(dir.normalized(), up.normalized());
}

void Building::init(Scene& scene)
{
    this->setScene(&scene);
    selectionMarkerMesh->setScene(&scene);
    selectionMarkerMesh->init(pos.to2());

    auto terrain = scene.getTerrain();

    plant(*terrain);

    auto avgH = getAverageElevation(*terrain);
    for(int x = 0; x <= length; x++)
    {
        for(int y = 0; y <= width; y++)
        {
            terrain->setElevation(x + pos.x-real(length)/2, y + pos.y-real(width)/2, avgH);
        }
    }

}

void Building::update(real dt)
{
    //geoPos = pos.to2();
    std::cout << pos << std::endl;
    selectionMarkerMesh->update(pos.to2());
}

bool Building::buildingWithin(real posX, real posY, int length, int width)
{
    //bool withinX = pos.x - real(this->length)/2 < posX + real(length)/2 && pos.x + real(this->length)/2 > posX - real(length)/2;
    //bool withinY = pos.y - real(this->width)/2 < posY + real(width)/2 && pos.y + real(this->width)/2 > posY - real(width)/2;
    bool withinX = 2*pos.x - this->length < 2*posX + length && 2*pos.x + this->length > 2*posX - length;
    bool withinY = 2*pos.y - this->width < 2*posY + width && 2*pos.y + this->width > 2*posY - width;
    return withinX && withinY;
}


bool Building::pointWithinFootprint(int posX, int posY)
{
    int x = posX - pos.x + real(length)/2, y = posY - pos.y + real(width)/2;
    return x >= 0 && x <= length && y >= 0 && y <= width && footprint[y*(length+1)+x];
}


bool Building::canBePlaced(real posX, real posY, int length, int width, Scene* scene)
{
    auto terrain = scene->getTerrain();
    for(auto building : scene->getBuildings())
    {
        if(building->buildingWithin(posX, posY, length, width))
            return false;
    }

    auto avgH = getAverageElevation(*terrain);
    auto fp = Building::footprint;
        
    std::cout << "\n\n\nPosition " << Vector2(posX, posY) << " | " << avgH << std::endl;
    bool canPlace = true;
    for(int x = 0; x < length; x++)
    {
        for(int y = 0; y < width; y++)
        {
            auto h = terrain->getElevation(x + posX-real(length/2), y + posY-real(width/2));
            std::cout << h << " ";
            if(std::abs(h - avgH) > 0.25)
            {
                canPlace = false;
            }
        }
        std::cout << std::endl;
    }

    return canPlace;
}


Entity* Building::spawnWreck()
{
    return nullptr;
}
