#include <vector>
#include <fstream>
#include <algorithm>
#include <tuple>


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