#pragma once
#pragma once

#include "Camera.h"
#include "Material.h"
#include "Mesh.h"
#include "Mesh3d.h"
#include "Scene.h"
#include "ShaderProgram.h"
#include "Vector3.h"
#include "Vertex3d.h"
#include <vector>

class LineMesh3d : public Mesh3d
{
public:
    ~LineMesh3d();

    std::vector<Vector3> v;
    std::vector<std::pair<int, int>> lines;
    std::vector<GLuint> indices;

    static Shader* vertexShader;
    static Shader* geometryShader;

    void init(Scene* scene);
    void tearDown(Scene* scene);

    Shader* getVertexShader() const;
    Shader* getGeometryShader() const;

    void draw();
    void updateUniforms();

    int lineWidth;

    LineMesh3d(std::vector<Vector3> vertices, std::vector<std::pair<int, int>> lines, Material* material, int lineWidth = 1);
    LineMesh3d();
};
