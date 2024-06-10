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
public:
    std::vector<Mesh3d*> meshes;
    bool _isTemplate;

public:
    static Model3d* createModel(const std::string& filename);
    //Model3d(std::string filename);
    Model3d(Mesh3d& mesh);
    Model3d();
    Model3d(Model3d& model);
    virtual ~Model3d();

    Model3d& operator= (const Model3d& model);

    std::map<std::string, Material*> readMaterialFile(const std::string& matfilestr);
    void readFromFile(const std::string& file);

    const std::vector<Mesh3d*>& getMeshes() const;

    void init(Scene* scene);
    void tearDown(Scene* scene);

    void draw();
    void addMesh(Mesh3d& mesh);

    void updateUniforms();

    void setPosition(Vector3 pos);

    void setDirection(Vector3 dir, Vector3 up);

    bool isTemplate()
    {
        return _isTemplate;
    }
};


class ModelManager
{
public:
    static bool hasModel(const std::string& str);
    //static void addModel(const std::string& identifier, Model3d& model);

    template<typename ...args> static Model3d* addModel(const std::string& identifier, Model3d* model)
    {
        templateMap[identifier] = model;
        for(auto mesh : model->meshes)
            mesh->isTemplate = true;
        model->_isTemplate = true;
        return model;
    }

    template<typename ...Args> static Model3d* addModel(const std::string& identifier, Args&&... args)
    {
        Model3d* model = new Model3d(std::forward<Args>(args)...);
        model->_isTemplate = true;
        templateMap[identifier] = model;
        for(auto mesh : model->meshes)
            mesh->isTemplate = true;
        return model;
    }

    static Model3d* cloneModel(const std::string& str);
    static void deleteModel(const std::string& str, Model3d* model);

protected:
    static std::unordered_map<std::string, Model3d*> templateMap;
};
