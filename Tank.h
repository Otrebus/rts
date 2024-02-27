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
#include "Entity.h"
#include "Model3d.h"


class Model3d;
class Scene;


class Tank : public Entity
{
public:
    Tank(Vector3 pos, Vector3 dir, Vector3 up, real width);
    virtual ~Tank();

    virtual void setUp(Scene* scene);
    virtual void updateUniforms();

    virtual void draw();

    void setPosition(Vector3 pos);
    void setDirection(Vector3 dir, Vector3 up);

    Model3d* body;
    Model3d* turret;
    Model3d* gun;
};
