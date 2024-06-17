#pragma once
#include "Mesh3d.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Matrix4.h"
#include "ShaderProgram.h"
#include "Material.h"
#include "Camera.h"
#include "Mesh.h"
#include <map>
#include "Model3d.h"
#include "BoundingBox.h"


class BoundingBoxModel : public Model3d
{

public:
    BoundingBoxModel(real width, real depth, real height);
    ~BoundingBoxModel();
};
