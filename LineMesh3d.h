#pragma once
#pragma once

#include <vector>
#include "Vertex3d.h"
#include "Material.h"
#include "ShaderProgram.h"
#include "Vector3.h"
#include "Camera.h"
#include "Scene.h"
#include "Mesh3d.h"
#include "Mesh.h"

class LineMesh3d : public Mesh
{
public:
    ~LineMesh3d();

    std::vector<Vector3> v;
    std::vector<std::pair<int, int>> lines;
    std::vector<GLuint> indices;

    static Shader* vertexShader;
    static Shader* geometryShader;

    void setUp(Scene* scene);
    void tearDown(Scene* scene);

    Shader* getVertexShader() const;
    Shader* getGeometryShader() const;

    void draw();
    void updateUniforms();

    int lineWidth;

    LineMesh3d(std::vector<Vector3> vertices, std::vector<std::pair<int, int>> lines, Material* material, int lineWidth = 1);
    LineMesh3d();
};
