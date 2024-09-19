#include "Bytestream.h"

Bytestream::Bytestream() : first(0), fail(false)
{
}

Bytestream::~Bytestream()
{
}

void Bytestream::SaveToFile(std::string fileName)
{
    std::ofstream file;
    file.open(fileName, std::ios::out | std::ios::trunc | std::ios::binary);
    for(auto byte : data)
        file.write(&byte, sizeof(char));
}

void Bytestream::LoadFromFile(std::string fileName)
{
    std::ifstream file;
    file.open(fileName, std::ios::in | std::ios::binary);

    for(char byte; !file.eof(); data.push_back(byte))
        file.read(&byte, sizeof(char));
}
