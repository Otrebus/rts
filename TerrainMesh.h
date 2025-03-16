#pragma once

#include "Mesh3d.h"

class TerrainMesh : public Mesh3d
{
public:
    TerrainMesh(std::vector<MeshVertex3> meshData, std::vector<int> triangles, Material* material);
    ~TerrainMesh();

    void updateElevation(int i, real height);

    static Shader* terrainVertexShader;

    void updateUniforms();

    Shader* getVertexShader() const;

    std::vector<MeshVertex3> v;

    using Mesh3d::Mesh3d;

    void setFlat(bool flatness);
    void draw(Material* mat);
    void init();
};
