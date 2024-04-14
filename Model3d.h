#pragma once
#include "Mesh3d.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Matrix4.h"
#include "ShaderProgram.h"
#include "Material.h"
#include "Camera.h"
#include "Mesh.h"
#include <map>


class Model3d
{
    std::vector<Mesh*> meshes;
    std::vector<Material*> materials;

public:
    Model3d(std::string filename);
    Model3d(Mesh& mesh);
    Model3d();
    ~Model3d();

    std::map<std::string, Material*> readMaterialFile(const std::string& matfilestr);
    void readFromFile(const std::string& file);

    const std::vector<Mesh*>& getMeshes() const;

    void init(Scene* scene);
    void tearDown(Scene* scene);

    void draw();
    void addMesh(Mesh& mesh);

    void updateUniforms();

    void setPosition(Vector3 pos);
    void setDirection(Vector3 dir, Vector3 up);
};
