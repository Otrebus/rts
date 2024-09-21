#pragma once
#include "Bytestream.h"
#include <fstream>

#pragma once

#include <vector>
#include <string>


class Bytestream
{
public:
    Bytestream();
    ~Bytestream();

    template<typename T> Bytestream& operator<<(const T& t)
    {
        char* p = (char*)&t;

        for(int i = 0; i < sizeof(T); i++)
            data.push_back(p[i]);

        if(data.size() - first < first)
        {
            for(unsigned int i = 0; i < data.size() - first; i++)
                data[i] = data[first + i];
        }
        return *this;
    }

    template<typename T> Bytestream& operator>>(T& t)
    {
        char* p = (char*)&t;

        if(first + sizeof(t) > data.size())
        {
            fail = true;
            return *this;
        }

        for(int i = 0; i < sizeof(t); i++)
            p[i] = data[first + i];
        first += sizeof(t);
        return *this;
    }

    void saveToFile(std::string fileName);
    void loadFromFile(std::string fileName);

private:
    bool fail;
    unsigned int first;
    std::vector<char> data;
};