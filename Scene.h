#pragma once

#include "Camera.h"

class Scene
{
public:
    Scene(Camera* camera);
    Camera* GetCamera() const;
private:
    Camera* camera;
};
