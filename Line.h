#pragma once

#include "Scene.h"
#include "Vector2.h"
#include "Vector3.h"


class Line
{
public:
    Line();
    Line(const std::vector<Vector3>& vertices);
    void setColor(Vector4 color);

public:
    GLuint VAO, VBO;

    static Shader* fragmentShader;
    static Shader* vertexShader;
    static Shader* geometryShader;
    Vector4 color;

    std::vector<float> vertexData;
    Scene* scene;

    ShaderProgram* program;
};


class Line3d : public Line
{
public:
    Line3d();
    Line3d(const std::vector<Vector3>& vertices);
    ~Line3d();

    void setDashed(bool b);
    void init(Scene* scene);
    void draw();
    void tearDown();
    void setVertices(const std::vector<Vector3>& vertices);
    void setInFront(bool); // TODO: dumb

private:
    bool inFront;
    bool dashed;
};


class Line2d : public Line
{
public:
    Line2d();
    Line2d(const std::vector<Vector2>& vertices);
    ~Line2d();

    void init(Scene* scene);
    void setVertices(const std::vector<Vector2>& vertices);
    void draw();
    void tearDown();
};
