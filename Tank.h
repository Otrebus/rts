#pragma once

#define NOMINMAX
#include "Line.h"
#include "Math.h"
#include "Unit.h"


class Tank : public Unit
{
public:
    Tank(Vector3 pos, Vector3 dir, Vector3 up, Terrain* terrain);
    ~Tank();

    void init(Scene* scene);
    void updateUniforms();

    void drawTurret(Material* mat = nullptr);
    void draw(Material* mat = nullptr);

    void setPosition(Vector3 pos);
    void setDirection(Vector3 dir, Vector3 up);

    void setTurretAbsoluteTarget(Vector3 target);
    bool canTurretAbsoluteTarget(Vector3 target);

    void accelerate(Vector2 dir);
    void brake();
    void turn(bool dir);

    void updateTurret(real dt);
    void update(real dt);

    Entity* spawnWreck();

    void shoot();

    bool setBallisticTarget(Unit* enemyTarget);

    static void loadModels();

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

    const real bulletSpeed = 5;
    const real fireInterval = 1.5;

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

    static real gunLength;
    real gunRecoilPos;

    Vector3 absGunDir;
    Vector3 absGunUp;
    Vector3 absGunPos;
    Vector3 absTurUp;
    Vector3 absTurDir;
    Vector3 absMuzzlePos;
    Vector3 absTurPos;

    Vector3 target;

    const real gunRecoil = 0.2f;
    const real gunRecoilRecoveryRate = 0.5f;

    Line3d destinationLine;


    static Model3d* tankBoundingBoxModel;
    static BoundingBox tankBoundingBox;
};
