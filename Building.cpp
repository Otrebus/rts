#pragma once

#include "Building.h"
#include "Scene.h"
#include "ShapeDrawer.h"
#include "BoundingBox.h"
#include "SelectionMarkerMesh.h"


Building::Building(Vector3 pos, int length, int width, std::vector<int> footprint) : Unit(pos, { 0, 1, 0 }, { 0, 0, 1 }), length(length), width(width), footprint(footprint)
{
    geoDir = dir.to2();
    geoPos = pos.to2();
    boundingBox = BoundingBox(Vector3(-real(length)/2, -real(width)/2, 0.0), Vector3(real(length)/2, real(width)/2, 2.0));

    selectionMarkerMesh = new SelectionMarkerMesh(3, 3);
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
    ShapeDrawer::drawBox(Vector3(int(pos.x), pos.y, pos.z) + Vector3(length - real(W)/2, real(width)/2, real(height)*2.0/3/2), Vector3(1, 0, 0), W, width, height*2.0/3, Vector3(0.7, 0.7, 0.7));
    ShapeDrawer::drawBox(Vector3(int(pos.x), pos.y, pos.z) + Vector3(real(length)/2, width - real(L)/2, real(height)/2), Vector3(1, 0, 0), length-W*2, L, height, Vector3(0.7, 0.7, 0.7));
    ShapeDrawer::drawBox(Vector3(int(pos.x), pos.y, pos.z) + Vector3(real(W)/2, real(width)/2, real(height)*2.0/3/2), Vector3(1, 0, 0), W, width, height*2.0/3, Vector3(0.7, 0.7, 0.7));
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


Entity* Building::spawnWreck()
{
    return nullptr;
}