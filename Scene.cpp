#include "Camera.h"
#include "Scene.h"

Scene::Scene(Camera* camera) : camera(camera)
{
}

Camera* Scene::GetCamera() const
{
    return camera;
}
