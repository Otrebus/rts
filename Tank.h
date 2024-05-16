#pragma once

#define NOMINMAX
#include "Unit.h"
#include "Math.h"
#include "Line.h"


class Model3d;
class Scene;
class BoundingBoxModel;
class Line3d;


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
    bool canTurretAbsoluteTarget(Vector3 target);

    void accelerate(Vector2 dir);
    void brake();
    void turn(bool dir);

    void updateTurret(real dt);
    void update(real dt);

    void shoot();

    bool setBallisticTarget(Unit* enemyTarget);

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

    const real minTurretPitch = -10_deg;
    const real maxTurretPitch = 90_deg; // Exaggerated for now

    const real bulletSpeed = 5.0;

    const real fireInterval = 2.5;

    real lastFired;

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
