#pragma once
#include "Mesh3d.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Matrix4.h"
#include "ShaderProgram.h"
#include "Material.h"
#include "Camera.h"
#include <map>


class Model3d
{
    std::vector<Mesh3d> meshes;
    std::vector<Material*> materials;

public:
    Model3d(std::string filename);
    Model3d(const Mesh3d& mesh);
    Model3d();
    ~Model3d();

    std::map<std::string, Material*> ReadMaterialFile(const std::string& matfilestr);
    void ReadFromFile(const std::string& file);

    void Setup(Scene* scene);
    void TearDown(Scene* scene);

    void Draw();
    void AddMesh(const Mesh3d& mesh);

    void UpdateUniforms();
};
