#pragma once

#pragma warning( disable : 4996 )

#define LOG_FILENAME "debuglog"
#define NOMINMAX
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <string>

class Logger
{    
public:
    Logger(const std::string&);
    Logger();
    ~Logger();

    static void box(const std::string&);
    void file(const std::string&);
private:
    std::string m_fileName;
};

extern Logger logger;

void checkError();