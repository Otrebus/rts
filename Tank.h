#pragma once

#include "Line.h"
#include "Math.h"
#include "Unit.h"
#include "ConsoleSettings.h"

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

    static ConsoleVariable tankMaxTurnRate;
    static ConsoleVariable tankMaxRadialAcc;

    static ConsoleVariable tankMaxSpeed;
    static ConsoleVariable tankMaxForwardAcc;
    static ConsoleVariable tankMaxBreakAcc;

    static ConsoleVariable tankBulletSpeed;
    static ConsoleVariable tankFireInterval;

    static ConsoleVariable tankTurretYawRate;
    static ConsoleVariable tankTurretPitchRate;

    static ConsoleVariable tankMinTurretPitch;
    static ConsoleVariable tankMaxTurretPitch;

    real lastFired;

    Terrain* terrain;
    real acceleration;
    real turnRate;

    Vector2 velocityTarget;
    Vector2 accelerationTarget;

    Vector3 turretTarget; // Relative direction
    Vector3 turretDir; // Relative to forward/up of the tank
    const static Vector3 turretPos;
    const static Vector3 gunPos;

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

    static BoundingBox tankBoundingBox;

    static ConsoleVariable boidDebug;

    static Material* fowMaterial;

    bool constructing;
    real constructionProgress;
};
