#pragma once

#include "Utils.h"

class Vector3;
class Model3d;
class Scene;

class ShapeDrawer
{
public:
    ShapeDrawer();

    static void drawArrow(Vector3 pos, Vector3 dir, real length, real width);
    static void drawBox(Vector3 pos, Vector3 dir, real length, real width, real height);
    static void drawSphere(Vector3 pos, real radius);
    static void drawCylinder(Vector3 pos, Vector3 dir, real length, real radius);
    static void loadModels();
    static void setScene(Scene* scene);

    static Model3d* cylinderModel;
    static Model3d* coneModel;
    static Model3d* sphereModel;
    static Model3d* boxModel;
    static Scene* scene;
};
