#pragma once

#include "Material.h"
#include "Mesh.h"
#include "Vertex3d.h"

class Mesh3d : public Mesh
{
public:
    std::vector<Vertex3> v;
    std::vector<int> triangles;

    static Shader* vertexShader;
    static Shader* geometryShader;

    Mesh3d(std::vector<Vertex3> vertices, std::vector<int> triangles, Material* material);
    Mesh3d();
    virtual ~Mesh3d();
    Mesh3d(Mesh3d& mesh);

    virtual void init();
    virtual void tearDown();

    virtual Shader* getVertexShader() const;
    virtual Shader* getGeometryShader() const;

    virtual void draw(Material* mat = nullptr);
    virtual void updateUniforms(Material* mat = nullptr);

    void transform(Matrix4 m);
};
