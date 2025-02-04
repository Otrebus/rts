#pragma once

template<typename T> struct Buffer2d
{
    std::vector<T> data;
    int width, height;

    Buffer2d()
    {
    }

    Buffer2d(int width, int height, T val)
    {
        this->width = width;
        this->height = height;
        data = std::vector<T>(width*height, val);
    }

    Buffer2d(int width, int height)
    {
        this->width = width;
        this->height = height;
        data = std::vector<T>(width*height);
    }

    Buffer2d(std::vector<T> data, int width, int height)
    {
        this->width = width;
        this->height = height;
        this->data = data;
    }

    T& get(int x, int y)
    {
        assert(x < width && x >= 0 && y < height && y >= 0);
        return data[y*width+x];
    }

    T& get(int x, int y, T overflowValue)
    {
        if(!(x < width && x >= 0 && y < height && y >= 0))
            return overflowValue;
        return data[y*width+x];
    }
};
