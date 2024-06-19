#pragma once
#include "BoundingBox.h"
#include "Camera.h"
#include "Material.h"
#include "Matrix4.h"
#include "Mesh.h"
#include "Mesh3d.h"
#include "Model3d.h"
#include "ShaderProgram.h"
#include "Vector2.h"
#include "Vector3.h"
#include <map>


class BoundingBoxModel : public Model3d
{

public:
    BoundingBoxModel(real width, real depth, real height);
    ~BoundingBoxModel();
};
