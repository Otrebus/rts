#pragma once
#pragma once

#include <vector>
#include "Vertex3d.h"
#include "Material.h"
#include "ShaderProgram.h"
#include "Vector3.h"
#include "Camera.h"
#include "Scene.h"
#include "Mesh3d.h"

class LineMesh3d : public Mesh3d
{
public:

    std::vector<Vector3> v;
    std::vector<std::pair<int, int>> lines;
    std::vector<GLuint> indices;
    Material* material;

    static Shader* vertexShader;
    static Shader* geometryShader;

    Vector3 dir, up, pos;

    GLuint VBO, VAO, EBO;
    Scene* scene;

    LineMesh3d(std::vector<Vector3> vertices, std::vector<std::pair<int, int>> lines, Material* material);
    LineMesh3d();

    virtual void setUp(Scene* scene);
    virtual void tearDown(Scene* scene);

    virtual Shader* getVertexShader() const;
    virtual Shader* getGeometryShader() const;

    virtual void draw();
    virtual void updateUniforms();

    void setDirection(Vector3 dir, Vector3 up);
    void setPosition(Vector3 pos);

    Material* getMaterial() const;
};
