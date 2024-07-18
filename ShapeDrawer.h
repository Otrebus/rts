#pragma once

class Vector3;
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
    static Model3d* sphereModel;
    static Scene* scene;
};
