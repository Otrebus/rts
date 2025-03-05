#include "Entity.h"
#include <fstream>
#include <iomanip>
#include <algorithm>


std::tuple<int, int, int> vectorToRgb(const Vector3& color)
{
    return { int(255*color.z), int(255*color.y), int(255*color.x) };
}


Vector3 rgbToVector(unsigned char r, unsigned char g, unsigned char b)
{
    return Vector3{ real(r)/255+(real)1e-6, real(g)/255+(real)1e-6, real(b)/255+(real)1e-6 };
}


std::tuple<std::vector<unsigned char>, int, int> readBMP(std::string filename, bool pad)
{
    std::ifstream file;
    file.open(filename, std::ios::binary);

    char info[54];
    file.read(info, 54);

    int width = *(int*)&info[18];
    int height = *(int*)&info[22];

    int widthPadded = (width*3 + 3) & (~3);

    int outWidth = pad ? widthPadded : width*3;
    std::vector<unsigned char> out(widthPadded*height);
    std::vector<char> buf(widthPadded*height);
    file.read(buf.data(), widthPadded*height);

    for(int y = height-1; y >= 0; y--)
    {
        for(int x = 0; x < width*3; x += 3)
        {
            int i = (height-1-y)*widthPadded + x;
            //out.insert(out.end(), { buf[i+2], buf[i+1], buf[i] } );
            out[outWidth*y+x] = buf[i+2];
            out[outWidth*y+x+1] = buf[i+1];
            out[outWidth*y+x+2] = buf[i];
        }
    }

    return { out, width, height };
}


void writeBMP(std::vector<Vector3> v, int width, int height, std::string filename)
{
    std::ofstream file;
    file.open(filename, std::ios::binary);

    auto pad = (4 - width*3%4)%4;
    auto size = (width*3 + pad)*height;

    file.write("BM", 2);
    int header[] = { size+54, 0, 54, 40, width, height, 1572865, 0, size, 0, 0, 0, 0 };
    for(auto x : header)
        for(int i = 0; i < 4; i++)
            file.put(0xFF &(x >> (i*8)));

    for(int y = height - 1; y >= 0; y--)
    {
        for(int x = 0; x < width; x++)
        {
            auto [r, g, b] = vectorToRgb(v[y*width+x]);
            file.put(r); file.put(g); file.put(b);
        }
        for(int n = 0; n < pad; n++)
            file.put(0);
    }
}


Vector2 mouseCoordToScreenCoord(int xres, int yres, int mouseX, int mouseY)
{
    return { real(2*mouseX)/xres - 1,  -(real(2*mouseY)/yres - 1) };
}


std::string realToString(real num, int significantDigits)
{
    std::stringstream ss;
    real d = significantDigits - std::log10(num);
    ss << std::fixed << std::setprecision(int(std::max(0.f, d))) << num;
    return ss.str();
}


std::string lower(const std::string& str)
{
    std::string output(str.size(), ' ');
    std::transform(str.begin(), str.end(), output.begin(), [](auto c){ return std::tolower(c); });
    return output;
}
