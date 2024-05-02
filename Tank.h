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
#include "Unit.h"
#include <random>


class Model3d;
class Scene;
class BoundingBoxModel;


class Tank : public Unit
{
public:
    Tank(Vector3 pos, Vector3 dir, Vector3 up, real width, Terrain* terrain);
    ~Tank();

    void init(Scene* scene);
    void updateUniforms();

    void drawTurret();
    void draw();

    void setPosition(Vector3 pos);
    void setDirection(Vector3 dir, Vector3 up);

    void setTurretAbsoluteTarget(Vector3 target);

    void accelerate(Vector2 dir);
    void brake();
    void turn(bool dir);

    void updateTurret(real dt);
    void update(real dt);

    void shoot();

    Vector2 seek();
    Vector2 evade();
    Vector2 avoid();
    Vector2 separate();
    Vector2 boidCalc();

    Model3d* body;
    Model3d* turret;
    Model3d* gun;

    const real maxTurnRate = 1.2*pi/4;
    const real maxRadialAcc = 4;
    const real maxTurnAcc = 2000;

    const real maxSpeed = 2;
    const real maxForwardAcc = 0.7; // TODO: breakacc vs forwardacc
    const real maxBreakAcc = 3;

    const real turretYawRate = pi/4;
    const real turretPitchRate = 0.25*pi/4;

    Terrain* terrain;
    real acceleration;
    real turnRate;

    Vector2 velocityTarget;
    Vector2 accelerationTarget;

    Vector3 turretTarget; // Relative direction
    Vector3 turretDir; // Relative to forward/up of the tank
    Vector3 turretPos;
    Vector3 gunPos;

    real gunLength;

    Line3d destinationLine;
    Line3d enemyLine;
};
