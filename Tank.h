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
#include "Math.h"
#include "Line.h"
#include <random>


class Model3d;
class Scene;
class BoundingBoxModel;


class Tank : public Entity
{
public:
    Tank(Vector3 pos, Vector3 dir, Vector3 up, real width, Terrain* terrain);
    virtual ~Tank();

    virtual void setUp(Scene* scene);
    virtual void updateUniforms();

    virtual void draw();

    void setPosition(Vector3 pos);
    void setDirection(Vector3 dir, Vector3 up);

    void accelerate(Vector2 dir);
    void brake();
    void turn(bool dir);

    void update(real dt);

    Vector2 seek();
    Vector2 evade();
    Vector2 avoid();
    Vector2 separate();
    Vector2 boidCalc();

    Model3d* body;
    Model3d* turret;
    Model3d* gun;

    const real maxTurnRate = 1.2*pi/4;
    const real maxTurnAcc = 4;
    const real maxSpeed = 2;
    const real maxForwardAcc = 0.7; // TODO: breakacc vs forwardacc
    const real maxBreakAcc = 15.5;

    Scene* scene;
    Terrain* terrain;
    real acceleration;
    real turnRate;

    Line3d destinationLine;
};
