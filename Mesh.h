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
    virtual ~Mesh();

    static Shader* vertexShader;
    static Shader* geometryShader;

    GLuint VBO, VAO, EBO;
    Scene* scene;

    void setScene(Scene* scene);
    virtual void tearDown() = 0;

    virtual Shader* getVertexShader() const = 0;
    virtual Shader* getGeometryShader() const = 0;

    virtual void draw() = 0;
    virtual void updateUniforms() = 0;

    void setDirection(Vector3 dir, Vector3 up);
    void setPosition(Vector3 pos);

    Matrix4 getTransformationMatrix();

    Material* getMaterial() const;

    Vector3 getUp() const;
    Vector3 getDir() const;
    Vector3 getPos() const;

private:
    Vector3 dir, up, pos;
    Matrix4 transformationMatrix;
    bool matrixCached;
};
