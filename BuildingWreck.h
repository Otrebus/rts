#pragma once
#include "Entity.h"
#include "Utils.h"

class BuildingWreck : public Entity
{
public:
    BuildingWreck(Vector3 pos, Vector3 dir, Vector3 up, int length, int width, Terrain* terrain);
    void updateUniforms();
    void draw(Material* mat = nullptr);
    void update(real dt);
    void init(Scene* scene);

    void setPosition(Vector3 pos);
    void setDirection(Vector3 dir, Vector3 up);
    
protected:
    friend class Building;

    static BoundingBox BuildingWreckBoundingBox;
    
    std::vector<int> footprint;

    int length, width, height;
    static Material* fowMaterial;
};

static BoundingBox BuildingWreckBoundingBox;
