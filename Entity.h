#pragma once

#include "BoundingBox.h"
#include "Utils.h"
#include "Vector2.h"
#include "Vector3.h"

class SelectionMarkerMesh;
class BoundingBox;
class Model3d;
class Scene;
class PathFindingRequest;
class Ray;
class Vector3;
class Vector2;
class Terrain;
class Material;

class Entity
{
public:
    Entity(Vector3 pos, Vector3 dir, Vector3 up);
    virtual ~Entity();

    void drawBoundingBox();
    bool intersectBoundingBox(const Ray& ray);
    bool intersectBoundingBox(Vector3 p1, Vector3 p2);
    std::pair<Vector3, Vector3> getBoundingBoxIntersection(Vector3 p1, Vector3 p2);

    virtual void init(Scene* scene);
    virtual void updateUniforms();

    virtual void plant(const Terrain& terrain);

    virtual void setPosition(Vector3 pos);
    Vector3 getPosition() const;
    virtual void setGeoPosition(Vector2 pos);
    Vector2 getGeoPosition() const;
    virtual void setDirection(Vector3 dir, Vector3 up);

    virtual void draw(Material* mat = nullptr) = 0;

    virtual void update(real dt) = 0;

    void setDead();

    void setVelocity(Vector3 velocity);
    Vector3 getVelocity() const;

    Vector2 getGeoVelocity() const;

    int getId() const;
    void setId(int id);

    Vector3 dir, up, pos;
    Vector3 velocity;

    int id;

    // The geographical position is not quite the same as the position of the tank projected
    // onto the xy plane, because e.g. if the tank is on a slope, the 3d position is the center
    // of its mass which is a little bit along the normal of the ground whereas the geographical
    // position is on teh ground
    Vector2 geoPos, geoDir, geoVelocity;

    BoundingBox boundingBox;

    Scene* scene;

    bool dead;
    real lastBumped;
};
