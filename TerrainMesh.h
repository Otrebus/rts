#pragma once

#include "Camera.h"
#include "Material.h"
#include "Mesh3d.h"
#include "Scene.h"
#include "ShaderProgram.h"
#include "Vector3.h"
#include "Vertex3d.h"
#include <vector>

class TerrainMesh : public Mesh3d
{
public:
    TerrainMesh(std::vector<MeshVertex3d> meshData, std::vector<int> triangles, Material* material);
    //TerrainMesh();
    ~TerrainMesh();

    static Shader* terrainVertexShader;

    void updateUniforms();

    Shader* getVertexShader() const;

    std::vector<MeshVertex3d> v;

    using Mesh3d::Mesh3d;

    void setFlat(bool flatness);
    void init();
};
