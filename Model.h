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

    std::map<std::string, Material*> readMaterialFile(const std::string& matfilestr);
    void readFromFile(const std::string& file);

    void setup(Scene* scene);
    void tearDown(Scene* scene);

    void draw();
    void addMesh(const Mesh3d& mesh);

    void updateUniforms();
};
