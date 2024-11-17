#pragma once

#include "Utils.h"
#include "Vector3.h"

class Model3d;
class Scene;
class LambertianMaterial;

class ShapeDrawer
{
public:
    ShapeDrawer();

    static void drawArrow(Vector3 pos, Vector3 dir, real length, real width, Vector3 color = { 0, 0, 0 });
    static void drawBox(Vector3 pos, Vector3 dir, real length, real width, real height, Vector3 color = { 0, 0, 0 });
    static void drawSphere(Vector3 pos, real radius, Vector3 color = { 0, 0, 0 });
    static void drawCylinder(Vector3 pos, Vector3 dir, real length, real radius, Vector3 color = { 0, 0, 0 });
    static void loadModels();
    static void setScene(Scene* scene);

    static void setDepthTest();
    static void restoreDepthTest();

    static void setInFront(bool);

    static Model3d* cylinderModel;
    static Model3d* coneModel;
    static Model3d* sphereModel;
    static Model3d* boxModel;
    static Scene* scene;
    static LambertianMaterial* material;

    static bool inFront;
    static bool depthTestIsEnabled;
};
