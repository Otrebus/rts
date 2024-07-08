#pragma once

#include "Camera.h"
#include "Material.h"
#include "Mesh.h"
#include "Scene.h"
#include "ShaderProgram.h"
#include "Vector3.h"
#include "Vertex3d.h"
#include <vector>

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

    virtual void draw(Material* mat = nullptr) = 0;
    virtual void updateUniforms(Material* mat = nullptr) = 0;

    void setDirection(Vector3 dir, Vector3 up);
    void setPosition(Vector3 pos);

    Matrix4 getTransformationMatrix();

    Material* getMaterial() const;

    Vector3 getUp() const;
    Vector3 getDir() const;
    Vector3 getPos() const;

    int nTriangles;
private:
    Vector3 dir, up, pos;
    Matrix4 transformationMatrix;
    bool matrixCached;
};
