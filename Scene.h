#pragma once

#include "Camera.h"

class Scene
{
public:
    Scene(Camera* camera);
    Camera* getCamera() const;
private:
    Camera* camera;
};
