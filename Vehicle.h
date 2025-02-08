#pragma once
#pragma once

#include "Line.h"
#include "Math.h"
#include "Unit.h"
#include "ConsoleSettings.h"

class Vehicle : public Unit
{
public:
    Vehicle(Vector3 pos, Vector3 dir, Vector3 up, Terrain* terrain);
    ~Vehicle();

    void init(Scene* scene);
    void updateUniforms();

    void drawTurret(Material* mat = nullptr);
    void draw(Material* mat = nullptr);

    void setPosition(Vector3 pos);
    void setDirection(Vector3 dir, Vector3 up);

    void accelerate(Vector2 dir);
    void brake();
    void turn(bool dir);
    void update(real dt);

    Entity* spawnWreck();

    Vector2 seek();
    Vector2 evade();
    Vector2 avoid();
    Vector2 separate();
    Vector2 boidCalc();

    Model3d* body;

    static ConsoleVariable vehicleMaxTurnAngle;
    static ConsoleVariable vehicleMaxRadialAcc;

    static ConsoleVariable vehicleMaxSpeed;
    static ConsoleVariable vehicleMaxForwardAcc;
    static ConsoleVariable vehicleMaxBreakAcc;

    Terrain* terrain;
    real acceleration;

    Vector2 velocityTarget;
    Vector2 accelerationTarget;

    Vector3 target;

    Line3d destinationLine;

    static BoundingBox vehicleBoundingBox;

    static ConsoleVariable boidDebug;

    static Material* fowMaterial;

    bool constructing;
    real constructionProgress;
};
