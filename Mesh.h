#pragma once
#pragma once

#include <vector>
#include "Vertex3d.h"
#include "Material.h"
#include "ShaderProgram.h"
#include "Vector3.h"
#include "Camera.h"
#include "Scene.h"
#include "Mesh.h"

class Mesh
{
public:
    Material* material;

    Mesh();

    static Shader* vertexShader;
    static Shader* geometryShader;

    Vector3 dir, up, pos;

    GLuint VBO, VAO, EBO;
    Scene* scene;

    virtual void setUp(Scene* scene) = 0;
    virtual void tearDown(Scene* scene) = 0;

    virtual Shader* getVertexShader() const = 0;
    virtual Shader* getGeometryShader() const = 0;

    virtual void draw() = 0;
    virtual void updateUniforms() = 0;

    void setDirection(Vector3 dir, Vector3 up);
    void setPosition(Vector3 pos);

    Material* getMaterial() const;
};
