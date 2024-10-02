#include "Bytestream.h"
#include <filesystem>

Bytestream::Bytestream() : first(0), fail(false)
{
}

Bytestream::~Bytestream()
{
}

void Bytestream::saveToFile(std::string fileName)
{
    std::ofstream file;
    file.open(fileName, std::ios::out | std::ios::trunc | std::ios::binary);
    file.write(data.data(), data.size());
}

void Bytestream::loadFromFile(std::string fileName)
{
    std::ifstream file;
    file.open(fileName, std::ios::in | std::ios::binary);

    auto size = std::filesystem::file_size(fileName);
    data.resize(size);
    file.read(data.data(), size);
}
