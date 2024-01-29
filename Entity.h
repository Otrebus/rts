#pragma once

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
    Entity();
    ~Entity();

    void drawBoundingBox();
    bool intersectBoundingBox(const Ray& ray);

    void setUp(Scene* scene);
    void updateUniforms();

    Vector3 dir;
    Vector3 up;
    Vector3 pos;

    Model3d* boundingBoxModel;

    BoundingBox bbox;
};
