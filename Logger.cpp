#include "Logger.h"
#define NOMINMAX
#include <windows.h>
#include <fstream>
#include <iostream>


Logger::Logger()
{
}


Logger::Logger(const std::string& fileName)
{
    m_fileName = fileName;
}


Logger::~Logger()
{
}


void Logger::box(const std::string& msg)
{
    std::wstring ws(msg.begin(), msg.end());
    MessageBox(0, ws.c_str(), (LPCWSTR)L"Error", MB_OK | MB_ICONERROR);
}


void Logger::file(const std::string& msg)
{
    std::ofstream file;
    time_t currentTime;
    time(&currentTime);
    char timestamp[100];

    file.open(m_fileName.c_str(), std::ofstream::app);
    if(file.fail())
    {
        box("Could not open debug log file.");
        exit(1);
    }

    sprintf(timestamp, "%s", ctime(&currentTime));
    timestamp[strlen(timestamp)-1] = 32;

    file << timestamp << ":: " << msg << std::endl;
    file.close();
}

void checkError()
{
    GLenum error;
    error = glGetError();

    if(error != GL_NO_ERROR)
        std::cout << "Not working" << std::endl;
    if(error == GL_INVALID_OPERATION)
        std::cout << "Invalid operation" << std::endl;
    if(error == GL_NO_ERROR)
        std::cout << "No error" << std::endl;
    if(error == GL_INVALID_VALUE)
        std::cout << "No value" << std::endl;
}


void GLAPIENTRY
MessageCallback(GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar* message,
                const void* userParam)
{
    if(severity > GL_DEBUG_SEVERITY_LOW)
        fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
                (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
                type, severity, message);
}
