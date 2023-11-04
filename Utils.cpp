#include <vector>
#include <fstream>
#include <algorithm>
#include <tuple>
#include "Matrix4.h"
#include "Vector3.h"
#include "Utils.h"


std::tuple<int, int, int> vectorToRgb(const Vector3& color)
{
    return { int(255*color.z), int(255*color.y), int(255*color.x) };
}


Vector3 rgbToVector(unsigned char r, unsigned char g, unsigned char b)
{
    return Vector3 { real(r)/255+(real)1e-6, real(g)/255+(real)1e-6, real(b)/255+(real)1e-6 };
}


std::tuple<std::vector<Vector3>, int, int> readBMP(std::string filename)
{
    std::ifstream file;
    file.open(filename, std::ios::binary);

    char info[54];
    file.read(info, 54);

    // extract image height and width from header
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];

    int widthPadded = (width*3 + 3) & (~3);
    std::vector<Vector3> out(width*height);
    // read the rest of the data at once
    std::vector<char> buf(widthPadded*height);
    file.read(buf.data(), widthPadded*height);

    for(int y = height-1; y >= 0; y--)
    {
        for(int x = 0; x < width; x++)
        {
            int i = (height-1-y)*widthPadded + x*3;
            out[y*height+x] = rgbToVector(buf[i+2], buf[i+1], buf[i]);
        }
    }

    return { out, width, height };
}

//std::tuple<std::vector<char>, int, int> readBMP(std::string filename)
//{
//    int i;
//    std::ifstream file;
//    file.open(filename, std::ios::binary);
//
//    char info[54];
//    file.read(info, 54);
//
//    // extract image height and width from header
//    int width = *(int*)&info[18];
//    int height = *(int*)&info[22];
//
//    std::vector<char> out(3*width*height);
//    // read the rest of the data at once
//    file.read(out.data(), 3*width*height);
//
//    for(i = 0; i < out.size(); i += 3)
//        std::swap(out[i], out[i+2]);
//
//    return { out, width, height };
//}

void writeBMP(std::vector<Vector3> v, int width, int height, std::string filename)
{
    std::ofstream file;
    file.open(filename, std::ios::binary);

    auto pad = (4 - width*3%4)%4;
    auto size = (width*3 + pad)*height;

    file.write("BM", 2);
    int header[] = { size + 54, 0, 54, 40, width, height, 1572865, 0, size, 0, 0, 0, 0 };
    for(auto x : header)
        for(int i = 0; i < 4; i++)
            file.put(0xFF &(x >> (i*8)));

    for(int y = height - 1; y >= 0; y--)
    {
        for(int x = 0; x < width; x++)
        {
            v[y*width*3+x];
            v[y*width*3+x+1];
            v[y*width*3+x+2];
        }
        for(int n = 0; n < pad; n++)
            file.put(0);
    }
}
