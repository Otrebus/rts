#pragma once
#include "Mesh3d.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Matrix4.h"
#include "ShaderProgram.h"
#include "Material.h"
#include "Camera.h"


class Model3d
{
    std::vector<Mesh3d> meshes;

public:
    Model3d(std::string filename);
    Model3d(const Mesh3d& mesh);
    Model3d();


    void Setup(Scene* scene);
    void Draw();
    void AddMesh(const Mesh3d& mesh);

    void UpdateUniforms();
};
