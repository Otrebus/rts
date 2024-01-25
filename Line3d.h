#pragma once

#include "Vector3.h"
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

    void setUp(Scene* scene);
    void draw();
    void tearDown();

private:
    GLuint VAO, VBO;
    std::vector<float> vertexData;
    Scene* scene;

    ShaderProgram* program;

    static Shader* fragmentShader;
    static Shader* vertexShader;
};
