#pragma once

#include "Vector3.h"
#include "Vector2.h"
#include <vector>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include "Scene.h"
#include "ShaderProgram.h"


class Line
{
public:
    Line();
    Line(const std::vector<Vector3>& vertices);
    void setColor(Vector3 color);

public:
    GLuint VAO, VBO;

    static Shader* fragmentShader;
    static Shader* vertexShader;
    Vector3 color;

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

    void init(Scene* scene);
    void draw();
    void tearDown();
    void setVertices(const std::vector<Vector3>& vertices);
    void setInFront(bool); // TODO: dumb

private:
    bool inFront;
};


class Line2d : public Line
{
public:
    Line2d();
    Line2d(const std::vector<Vector2>& vertices);
    ~Line2d();

    void init(Scene* scene);
    void draw();
    void tearDown();
};
