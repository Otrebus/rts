#pragma once

#include <vector>
#include "Vertex3d.h"
#include "Material.h"
#include "ShaderProgram.h"
#include "Vector3.h"
#include "Camera.h"
#include "Scene.h"

struct Mesh3d
{
    std::vector<Vertex3d> v;
    std::vector<int> triangles;
    Material* material;

    GLuint VBO, VAO, EBO;
    Scene* scene;

    Mesh3d(std::vector<Vertex3d> vertices, std::vector<int> triangles, Material* material);
    Mesh3d();

    void setup(Scene* scene);
    void tearDown(Scene* scene);

    void draw();
    void updateUniforms();
};
