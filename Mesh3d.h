#pragma once

#include <vector>
#include "Vertex3d.h"
#include "Material.h"
#include "ShaderProgram.h"
#include "Vector3.h"
#include "Camera.h"
#include "Scene.h"
#include "Mesh.h"

class Mesh3d : public Mesh
{
public:
    std::vector<Vertex3d> v;
    std::vector<int> triangles;

    static Shader* vertexShader;
    static Shader* geometryShader;

    Mesh3d(std::vector<Vertex3d> vertices, std::vector<int> triangles, Material* material);
    Mesh3d();

    virtual void setUp(Scene* scene);
    virtual void tearDown(Scene* scene);

    virtual Shader* getVertexShader() const;
    virtual Shader* getGeometryShader() const;

    void draw();
    void updateUniforms();
};
