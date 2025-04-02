#pragma once

#include "Mesh3d.h"

class LineMesh3d : public Mesh3d
{
public:
    ~LineMesh3d();

    std::vector<Vector3> v;
    std::vector<std::pair<int, int>> lines;
    std::vector<GLuint> indices;

    static Shader* vertexShader;
    static Shader* geometryShader;

    void init();
    void tearDown();

    Shader* getVertexShader() const;
    Shader* getGeometryShader() const;

    void draw(Material* mat = nullptr);
    void updateUniforms(Material* mat = nullptr);

    int lineWidth;

    LineMesh3d(std::vector<Vector3> vertices, std::vector<std::pair<int, int>> lines, Material* material = nullptr, int lineWidth = 1);
    LineMesh3d();
};
