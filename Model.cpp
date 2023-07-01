#include <vector>
#include "Vector2.h"
#include "Vector3.h"
#include "Model.h"
#include "LambertianMaterial.h"


void Mesh3d::Setup()
{
    ShaderProgram s;

    material = new LambertianMaterial(Vector3(0.5, 0.5, 0.2));

    Shader vertexShader("vertex.glsl", GL_VERTEX_SHADER); // This should probably be saved somewhere

    s.AddShaders(vertexShader, *material->GetShader());
    s.Use();

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex3d)*v.size(), v.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*triangles.size(), triangles.data(), GL_STATIC_DRAW);
}


void Mesh3d::Draw()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


void Model3d::Setup()
{
    for(auto& mesh : meshes)
        mesh.Setup();
}


void Model3d::Draw()
{
    for(auto& mesh : meshes)
        mesh.Draw();
}

void Model3d::SetTransformationMatrix(const Matrix4& m)
{

}