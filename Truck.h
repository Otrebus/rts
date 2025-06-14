#pragma once
#pragma once

#include "Line.h"
#include "Math.h"
#include "Unit.h"
#include "ConsoleSettings.h"

class Truck : public Unit
{
public:
    Truck(Vector3 pos, Vector3 dir, Vector3 up, Terrain* terrain);
    ~Truck();

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

    Vector2 calcSeekVector(Vector2 dest);

    void handleCommand(real dt);

    static void loadModels();

    Model3d* body;

    static ConsoleVariable turnRadius;
    static ConsoleVariable maxRadialAcc;

    static ConsoleVariable maxSpeed;
    static ConsoleVariable maxForwardAcc;
    static ConsoleVariable maxBreakAcc;

    Terrain* terrain;
    real acceleration;

    Vector2 velocityTarget;
    Vector2 accelerationTarget;

    Vector3 target;
    real turnRate;

    Line3d destinationLine;

    static BoundingBox truckBoundingBox;

    static Material* fowMaterial;

    Model3d* model;

    bool constructing;
    real constructionProgress;
};
