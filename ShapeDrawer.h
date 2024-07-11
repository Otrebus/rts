#pragma once

#define NOMINMAX
#include "Line.h"
#include "Ray.h"
#include "Vector2.h"

class Model3d;
class Scene;

class ShapeDrawer
{
public:
    ShapeDrawer();

    static void drawArrow(Vector3 pos, Vector3 dir);
    static void loadModels();
    static void setScene(Scene* scene);

    static Model3d* cylinderModel;
    static Model3d* coneModel;
    static Scene* scene;
};
