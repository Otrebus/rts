#include <vector>
#include "Vector2.h"
#include "Vector3.h"
#include "Model.h"
#include "LambertianMaterial.h"


void Mesh3d::Setup()
{
    program = new ShaderProgram();

    Shader vertexShader("vertex.glsl", GL_VERTEX_SHADER); // This should probably be saved somewhere

    program->AddShaders(vertexShader, *material->GetShader());
    program->Use();

    material->SetUniforms(program->GetId());
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
    program->Use();    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_INT, 0);
}


void Mesh3d::SetTransformationMatrix(const Matrix4& m)
{
    program->Use();   
    glUniformMatrix4fv(glGetUniformLocation(program->GetId(), "transform"), 1, GL_TRUE, (float*) m.m_val);
}

void Model3d::SetCameraPosition(const Vector3& v)
{
    for(auto& mesh : meshes)
        mesh.SetCameraPosition(v);    
}

void Mesh3d::SetCameraPosition(const Vector3& v)
{
    program->Use();
    glUniform3fv(glGetUniformLocation(program->GetId(), "camPos"), 1, (GLfloat*) &v);
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
    for(auto& mesh : meshes)
        mesh.SetTransformationMatrix(m);
}