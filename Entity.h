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


class Model3d;
class Scene;


class Entity {
public:
    Entity(Vector3 pos, Vector3 dir, Vector3 up);
    ~Entity();

    void drawBoundingBox();
    bool intersectBoundingBox(const Ray& ray);

    void setUp(Scene* scene);
    void updateUniforms();

    Vector3 dir;
    Vector3 up;
    Vector3 pos;

    void setSelected(bool selected);

    Model3d* boundingBoxModel;

    BoundingBox bbox;

    bool selected;
};
