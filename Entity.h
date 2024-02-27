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

class Model3d;
class Scene;

class Entity 
{
public:
    Entity(Vector3 pos, Vector3 dir, Vector3 up);
    ~Entity();

    void drawBoundingBox();
    bool intersectBoundingBox(const Ray& ray);

    void setUp(Scene* scene);
    void updateUniforms();

    void setSelected(bool selected);

    virtual void plant(const Terrain& terrain);

    virtual void setPosition(Vector3 pos);
    virtual void setDirection(Vector3 dir, Vector3 up);

    Vector3 dir, up, pos;

    Model3d* boundingBoxModel;
    Mesh* boundingBoxMesh;
    Mesh* lineMesh;

    BoundingBox bbox;

    bool selected;
};
