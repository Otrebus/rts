#pragma once
#pragma once

#include "Line.h"
#include "Math.h"
#include "Unit.h"
#include "ConsoleSettings.h"
#include "Rock.h"

class Harvester : public Unit
{
public:
    Harvester(Vector3 pos, Vector3 dir, Vector3 up, Terrain* terrain);
    ~Harvester();

    void init(Scene* scene);
    void updateUniforms();

    void draw(Material* mat = nullptr);

    void setPosition(Vector3 pos);
    void setDirection(Vector3 dir, Vector3 up);

    void accelerate(Vector2 dir);
    void brake();
    void turn(bool dir);
    void update(real dt);

    void handleCommand(real dt);

    std::pair<bool, int> getmoveDir(Vector2 dest, Vector2 geoDirection, Vector2 geoVelocity, real maxV, real a_f, real a_r, real a_b, real turnRadius);
    real getTime(real v, real a_f, real a_r, real maxV, real d);

    Entity* spawnWreck();

    Vector2 seek();
    Vector2 evade();
    Vector2 avoid();
    Vector2 separate();
    Vector2 boidCalc();

    Vector2 calcSeekVector(Vector2 dest);

    static void loadModels();

    Model3d* body;

    static ConsoleVariable turnRadius;
    static ConsoleVariable maxRadialAcc;

    static ConsoleVariable maxSpeed;
    static ConsoleVariable maxReverseSpeed;
    static ConsoleVariable maxForwardAcc;
    static ConsoleVariable maxBreakAcc;

    Terrain* terrain;
    real acceleration;

    Vector2 velocityTarget;
    Vector2 accelerationTarget;

    Vector3 target;
    real turnRate;

    Line3d destinationLine;

    static BoundingBox HarvesterBoundingBox;

    static Material* fowMaterial;

    Model3d* model;

    bool constructing;
    real constructionProgress;
};
