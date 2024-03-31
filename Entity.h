#pragma once

#define NOMINMAX
#include "Vector3.h"
#include <vector>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include "Scene.h"
#include "ShaderProgram.h"
#include "BoundingBox.h"
#include "Mesh3d.h"
#include "LineMesh3d.h"

class BoundingBoxModel;
class Model3d;
class Scene;

class Entity 
{
public:
    Entity(Vector3 pos, Vector3 dir, Vector3 up, real width = 0.3, real height = 0.3, real depth = 0.3);
    ~Entity();

    void drawBoundingBox();
    bool intersectBoundingBox(const Ray& ray);

    virtual void setUp(Scene* scene);
    virtual void updateUniforms();

    void setSelected(bool selected);

    virtual void plant(const Terrain& terrain);

    virtual void setPosition(Vector3 pos);
    Vector3 getPosition() const;
    virtual void setGeoPosition(Vector2 pos);
    Vector2 getGeoPosition() const;
    virtual void setDirection(Vector3 dir, Vector3 up);

    virtual void draw() = 0;

    virtual void update(real dt) = 0;


    void setTarget(Vector3 pos);
    Vector3 getTarget() const;

    void setPath(std::vector<Vector2> path);
    const std::vector<Vector2>& getPath() const;

    void setVelocity(Vector2 velocity);
    Vector2 getVelocity() const;

    Vector3 dir, up, pos;
    Vector3 target;
    real width, height, depth;
    Vector2 velocity;

    Vector2 geoPos, geoDir;

    BoundingBoxModel* boundingBoxModel;

    BoundingBox boundingBox;

    bool selected;
    std::vector<Vector2> path;
};
