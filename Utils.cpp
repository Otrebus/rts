#include <vector>
#include <fstream>
#include <algorithm>
#include <tuple>
#include "Matrix4.h"
#include "Vector3.h"


std::tuple<std::vector<char>, int, int> readBMP(std::string filename)
{
    int i;
    std::ifstream file;
    file.open(filename, std::ios::binary);

    char info[54];
    file.read(info, 54);

    // extract image height and width from header
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];

    std::vector<char> out(3*width*height);
    // read the rest of the data at once
    file.read(out.data(), 3*width*height);

    for(i = 0; i < out.size(); i += 3)
        std::swap(out[i], out[i+2]);

    return { out, width, height };
}


Matrix4<float> getCameraMatrix(Vector3<float> pos, Vector3<float> lookAt, float fov, float ar)
{
    float pi = 3.141592653589793;
    float a = std::tan(pi*fov/180/2);
    float n = -0.001, f = -1000.0;

    Matrix4<float> trans(
        1, 0, 0, -pos.x,
        0, 1, 0, -pos.y,
        0, 0, 1, -pos.z,
        0, 0, 0, 1
    );

    Vector3 d = (pos-lookAt).Normalized();
    Vector3 u1 = Vector3(0.f, 1.f, 0.f);
    Vector3 v = (u1^d).Normalized();
    Vector3 u = (d^v).Normalized();

    Matrix4<float> proj(
        v.x, v.y, v.z, 0,
        u.x, u.y, u.z, 0,
        d.x, d.y, d.z, 0,
        0, 0, 0, 1
    );

    Matrix4<float> persp(
        1.0/a, 0, 0, 0,
        0, ar/a, 0, 0,
        0, 0, -(-n-f)/(n-f), -2.0*f*n/(n-f),
        0, 0, -1, 0
    );

    return persp*proj*trans;
}