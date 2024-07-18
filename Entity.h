#pragma once

#include "BoundingBox.h"
#include "Utils.h"
#include "Vector2.h"
#include "Vector3.h"

class SelectionMarkerMesh;
class BoundingBox;
class BoundingBoxModel;
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

    // TODO: are these used? width seems to be zero
    //real width, height, depth;

    Vector2 geoPos, geoDir, geoVelocity;

    Model3d* boundingBoxModel;
    BoundingBox boundingBox;

    Scene* scene;

    bool dead;
};
