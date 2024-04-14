#pragma once

#include "Vector3.h"
#include "Vector2.h"
#include <vector>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include "Scene.h"
#include "ShaderProgram.h"

class Line3d {
public:
    Line3d();
    Line3d(const std::vector<Vector3>& vertices);
    ~Line3d();

    void init(Scene* scene);
    void draw();
    void tearDown();
    void setColor(Vector3 color);
    void setVertices(const std::vector<Vector3>& vertices);
    void setInFront(bool); // TODO: dumb

private:
    GLuint VAO, VBO;
    std::vector<float> vertexData;
    Scene* scene;

    ShaderProgram* program;

    bool inFront;
    static Shader* fragmentShader;
    static Shader* vertexShader;
    Vector3 color;
};


class Line2d {
public:
    Line2d();
    Line2d(const std::vector<Vector2>& vertices);
    ~Line2d();

    void init(Scene* scene);
    void draw();
    void tearDown();

private:
    GLuint VAO, VBO;
    std::vector<float> vertexData;
    Scene* scene;

    Vector3 color;
    ShaderProgram* program;

    static Shader* fragmentShader;
    static Shader* vertexShader;
};
