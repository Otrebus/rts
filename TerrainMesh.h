#pragma once

#include <vector>
#include "Vertex3d.h"
#include "Mesh3d.h"
#include "Material.h"
#include "ShaderProgram.h"
#include "Vector3.h"
#include "Camera.h"
#include "Scene.h"

class TerrainMesh : public Mesh3d
{
    //TerrainMesh(std::vector<Vertex3d> vertices, std::vector<int> triangles, Material* material);
    //TerrainMesh();

    using Mesh3d::Mesh3d;

    void setup(Scene* scene);
    //void tearDown(Scene* scene);

    //void draw();
    //void updateUniforms();
};
