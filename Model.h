#include "Vector3"
#include "Vector2"


template <typename T> class Vertex3d
{
    Vector3<T> pos;
    Vector3<T> normal;
    Vector2<T> tex;
};


template <typename T> class Mesh3d
{
    std::vector<3dVertex> v;
    std::vector<int> tri;
    Shader* shader;
};


template <typename T> class Model3d
{
    std::vector<Mesh3d> v;
};
