#pragma once

#include <vector>
#include "Vertex3d.h"
#include "Material.h"
#include "ShaderProgram.h"
#include "Vector3.h"
#include "Camera.h"
#include "Scene.h"

class Mesh3d
{
public:

    std::vector<Vertex3d> v;
    std::vector<int> triangles;
    Material* material;

    static Shader* vertexShader;
    static Shader* geometryShader;

    Vector3 dir, up, pos;

    GLuint VBO, VAO, EBO;
    Scene* scene;

    Mesh3d(std::vector<Vertex3d> vertices, std::vector<int> triangles, Material* material);
    Mesh3d();

    virtual void setUp(Scene* scene);
    virtual void tearDown(Scene* scene);

    virtual Shader* getVertexShader() const;
    Shader* getGeometryShader() const;

    virtual void draw();
    virtual void updateUniforms();

    void setDirection(Vector3 dir, Vector3 up);
    void setPosition(Vector3 pos);

    Material* getMaterial() const;
};
