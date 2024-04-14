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
public:
    TerrainMesh(std::vector<MeshVertex3d> meshData, std::vector<int> triangles, Material* material);
    //TerrainMesh();
    ~TerrainMesh();

    static Shader* terrainVertexShader;

    Shader* getVertexShader() const;

    std::vector<MeshVertex3d> v;

    using Mesh3d::Mesh3d;

    void init(Scene* scene);
};
