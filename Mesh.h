#pragma once

#include "Material.h"

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
    void setSize(Vector3 size);

    Matrix4 getTransformationMatrix();

    Material* getMaterial() const;

    Vector3 getUp() const;
    Vector3 getDir() const;
    Vector3 getPos() const;

    int nTriangles;
protected:
    Vector3 dir, up, pos, size;
    Matrix4 transformationMatrix;
    bool matrixCached;
};
