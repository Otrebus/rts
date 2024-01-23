#pragma once

#include "Vector3.h"
#include <vector>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include "Scene.h"
#include "ShaderProgram.h"
#include "BoundingBox.h"


class Entity {
public:
    Entity();
    ~Entity();

    void drawBoundingBound();

    Vector3 dir;
    Vector3 up;
    Vector3 pos;

    Mesh3d* boundingBoxMesh;

    BoundingBox bbox;
};
