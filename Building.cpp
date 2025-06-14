#pragma once

#include "Building.h"
#include "Scene.h"
#include "ShapeDrawer.h"
#include "BoundingBox.h"
#include "SelectionMarkerMesh.h"
#include "Terrain.h"
#include "FogOfWarMaterial.h"
#include "BuildingWreck.h"
#include "PathFinding.h"
#include "UserInterface.h"
#include "Tank.h"


Building::Building(int x, int y, int length, int width, std::vector<int> footprint) : Unit(pos, { 1, 0, 0 }, { 0, 0, 1 }), length(length), width(width), footprint(footprint)
{
    lastSpawnedParticle = 0;
    constructeeId = 0;
    height = 2;
    geoDir = dir.to2();
    pos = Vector3(x+real(length)/2, y+real(width)/2, 0);
    geoPos = pos.to2();
    boundingBox = BoundingBox(Vector3(-real(length)/2, -real(width)/2, -real(height)/2), Vector3(real(length)/2, real(width)/2, real(height)/2));
    selectionMarkerMesh = new SelectionMarkerMesh(length+0.2f, width+0.2f, false);
    if(!fowMaterial)
        fowMaterial = new FogOfWarMaterial();
}

Building::~Building()
{
}

void Building::draw(Material* mat = nullptr)
{
    real W = 0.1f, L = 1.0f;

    GLint curDepthFun;
    GLboolean curBlend;
    GLint curSrc, curDst;

    glGetIntegerv(GL_DEPTH_FUNC, &curDepthFun);
    glGetBooleanv(GL_BLEND, &curBlend);
    glGetIntegerv(GL_BLEND_SRC_RGB, &curSrc);
    glGetIntegerv(GL_BLEND_DST_RGB, &curDst);
    
    glBlendFuncSeparate(GL_ZERO, GL_ONE, GL_ONE, GL_ZERO);

    ShapeDrawer::setInFront(false);

    ShapeDrawer::drawBox(Vector3(pos.x, pos.y, pos.z) + Vector3(-real(W)/2+real(length)/2, 0.f, -real(height)/6), Vector3(1.f, 0.f, 0.f), up, W, real(width), height*2.0f/3, fowMaterial);
    ShapeDrawer::drawBox(Vector3(pos.x, pos.y, pos.z) + Vector3(real(W)/2-real(length)/2, 0.f, -real(height)/6), Vector3(1.f, 0.f, 0.f), up, W, real(width), height*2.0f/3, fowMaterial);
    ShapeDrawer::drawBox(Vector3(pos.x, pos.y, pos.z) + Vector3(0.f, real(width)/2 - real(L)/2, 0.f), Vector3(1, 0, 0), up, length-W*2, L, real(height), fowMaterial);

    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);

    ShapeDrawer::drawBox(Vector3(pos.x, pos.y, pos.z) + Vector3(-real(W)/2+real(length)/2, 0.f, -real(height)/6), Vector3(1.f, 0.f, 0.f), up, W, real(width), height*2.0f/3, Vector3(0.7f, 0.7f, 0.7f));
    ShapeDrawer::drawBox(Vector3(pos.x, pos.y, pos.z) + Vector3(real(W)/2-real(length)/2, 0.f, -real(height)/6), Vector3(1.f, 0.f, 0.f), up, W, real(width), height*2.0f/3, Vector3(0.7f, 0.7f, 0.7f));
    ShapeDrawer::drawBox(Vector3(pos.x, pos.y, pos.z) + Vector3(0.f, real(width)/2 - real(L)/2, 0.f), Vector3(1.0f, 0.f, 0.f), up, length-W*2, L, real(height), Vector3(0.7f, 0.7f, 0.7f));

    if(!curBlend)
        glDisable(GL_BLEND);
    glDepthFunc(curDepthFun);
    glBlendFunc(curSrc, curDst);
}

real Building::getAverageElevation(const Terrain& terrain)
{
    // This isn't really the total average but the average of the four corners
    auto x = Vector3(geoDir.x, geoDir.y, 0.f).normalized();
    auto y = Vector3(-geoDir.y, geoDir.x, 0.f).normalized();
    auto z = Vector3(0.f, 0.f, 1.f).normalized();

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
    auto h = terrain.getElevation(geoPos.x, geoPos.y);
    pos = Vector3(geoPos.x, geoPos.y, h+real(height)/2);
    setPosition(pos);
    setDirection(dir.normalized(), up.normalized());
}

void Building::init(Scene& scene)
{
    this->setScene(&scene);
    selectionMarkerMesh->setScene(&scene);
    selectionMarkerMesh->init(pos.to2());

    auto terrain = scene.getTerrain();

    auto avgH = getAverageElevation(*terrain);

    // If there are any buildings right next to us we set the height to that of that building
    // otherwise that building would get its footprint damaged. This assumes all the buildings
    // next to this one has the same height
    std::vector<real> adjHeights;
    auto cx = pos.x-real(length)/2, cy = pos.y-real(width)/2;
    for(int x = 0; x <= length; x++)
    {
        for(int y = 0; y <= width; y++)
        {
            if(auto h = terrain->getElevation(real(x) + cx, real(y) + cy); std::abs(h - avgH) > 0.25f)
            for(auto building : scene.getBuildings())
                if(building->pointWithin(int(real(x)+cx), int(real(y)+cy)))
                    adjHeights.push_back(terrain->getElevation(real(x) + cx, real(y) + cy));
        }
    }

    if(!adjHeights.empty())
        avgH = adjHeights[0];

    for(int x = 0; x <= length; x++)
        for(int y = 0; y <= width; y++)
            terrain->setElevation(int(x + pos.x-real(length)/2), int(y + pos.y-real(width)/2), avgH);

    plant(*terrain);
}

void Building::update(real dt)
{
    selectionMarkerMesh->update(pos.to2());
    if(auto constructee = scene->getEntity(constructeeId); constructee)
    {
        auto tank = dynamic_cast<Tank*>(constructee);
        if(tank->constructing && glfwGetTime() - lastSpawnedParticle > 0.01f)
        {
            auto gp = new ConstructionParticle(int(lastSpawnedParticle*100)%2 ? pos + dir : pos - dir, *tank, false);
            lastSpawnedParticle = glfwGetTime();
            scene->addParticle(gp);
        }
    }
}

bool Building::buildingWithin(real posX, real posY, int length, int width) const
{
    bool withinX = 2*pos.x - this->length < 2*posX + length && 2*pos.x + this->length > 2*posX - length;
    bool withinY = 2*pos.y - this->width < 2*posY + width && 2*pos.y + this->width > 2*posY - width;
    return withinX && withinY;
}

void Building::produceTank()
{
    auto tank = new Tank(pos, -dir.to2().perp().to3(), up, scene->getTerrain());
    tank->constructing = true;
    scene->addEntity(tank);
    tank->init(scene);
    tank->commandQueue.push(MoveCommand((pos + tank->dir.normalized()*3).to2()));

    constructeeId = tank->getId();
}

bool Building::pointWithinFootprint(int posX, int posY) const
{
    int x = int(posX - pos.x + real(length)/2), y = int(posY - pos.y + real(width)/2);
    return x >= 0 && x <= length && y >= 0 && y <= width && footprint[y*(length+1)+x];
}

bool Building::pointWithin(int posX, int posY) const
{
    int x = int(posX - pos.x + real(length)/2), y = int(posY - pos.y + real(width)/2);
    return x >= 0 && x <= length && y >= 0 && y <= width;
}

bool Building::canBePlaced(real posX, real posY, int length, int width, Scene* scene)
{
    auto terrain = scene->getTerrain();
    for(auto building : scene->getBuildings())
        if(building->buildingWithin(posX, posY, length, width))
            return false;

    auto avgH = getAverageElevation(*terrain);
    auto fp = Building::footprint;
        
    bool canPlace = true;
       
    std::vector<real> adjHeights;
    auto cx = posX-real(length)/2, cy = posY-real(width)/2;
    for(int x = 0; x <= length; x++)
    {
        for(int y = 0; y <= width; y++)
        {
            if(auto h = terrain->getElevation(x+cx, y+cy); std::abs(h-avgH) > 0.5)
                canPlace = false;
            for(auto building : scene->getBuildings())
            {
                if(building->pointWithin(int(x+cx), int(y+cy)))
                {
                    adjHeights.push_back(terrain->getElevation(x+cx, y+cy));
                }
            }
        }
    }

    if(!adjHeights.empty())
        for(auto h : adjHeights)
            if(h != adjHeights[0])
                return false;

    return canPlace;
}


Entity* Building::spawnWreck()
{
    auto tw = new BuildingWreck(pos, dir, up, length, width, scene->getTerrain());
    tw->length = length;
    tw->width = width;
    tw->init(scene);
    return tw;
}


std::vector<int> Building::getAbsoluteFootprint()
{
    std::vector<int> p;
    for(int y = 0; y <= width; y++)
    {
        for(int x = 0; x <= length; x++)
        {
            if(footprint[x*(length+1)+y])
            {
                auto X = int(pos.x - real(length)/2);
                auto Y = int(pos.y - real(width)/2);
                p.push_back((Y+y)*scene->getTerrain()->getWidth()+(X+x));
            }
        }
    }
    return p;
}


Material* Building::fowMaterial = nullptr;
