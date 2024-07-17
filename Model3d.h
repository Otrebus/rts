#pragma once
#include "Matrix4.h"
#include "Mesh.h"
#include "Mesh3d.h"
#include <map>


class Model3d
{
public:
    std::vector<Mesh3d*> meshes;
    bool _isTemplate;

public:
    //Model3d(std::string filename);
    Model3d(Mesh3d& mesh);
    Model3d();
    Model3d(Model3d& model);
    virtual ~Model3d();

    Model3d& operator= (const Model3d& model);

    std::map<std::string, Material*> readMaterialFile(const std::string& matfilestr);
    void readFromFile(const std::string& file);

    const std::vector<Mesh3d*>& getMeshes() const;

    void setScene(Scene* scene);
    void init();
    void tearDown(Scene* scene);

    void draw(Material* mat = nullptr);
    void addMesh(Mesh3d& mesh);

    void updateUniforms(Material* mat = nullptr);

    void setPosition(Vector3 pos);
    Vector3 getPosition();

    void setDirection(Vector3 dir, Vector3 up);
    Vector3 getDirection();
    Vector3 getUp();

    Vector3 pos, dir, up;

    bool cloned;
};


class ModelManager
{
public:
    static bool hasModel(const std::string& str);
    //static void addModel(const std::string& identifier, Model3d& model);

    template<typename ...args> static Model3d* addModel(const std::string& identifier, Model3d* model)
    {
        assert(!templateMap.contains(identifier));
        templateMap[identifier] = model;
        model->init();
        return model;
    }

    template<typename ...Args> static Model3d* addModel(const std::string& identifier, Args&&... args)
    {
        assert(!templateMap.contains(identifier));
        Model3d* model = new Model3d(std::forward<Args>(args)...);
        templateMap[identifier] = model;
        model->init();
        return model;
    }

    static Model3d* instantiateModel(const std::string& str);
    static void deleteModel(const std::string& str, Model3d* model);

    Model3d* createModel(const std::string& str);
    static Model3d* addModel(const std::string& filename, Model3d* model);
    static Model3d* getModel(const std::string& name);

protected:
    static std::unordered_map<std::string, Model3d*> templateMap;
};
