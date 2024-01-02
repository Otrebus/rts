#include "Camera.h"
#include "Scene.h"

Scene::Scene(Camera* camera) : camera(camera)
{
}

Camera* Scene::getCamera() const
{
    return camera;
}
