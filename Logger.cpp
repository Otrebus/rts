#include "Logger.h"
#define NOMINMAX
#include <windows.h>
#include <fstream>


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
    MessageBox(0, ws.c_str(), (LPCWSTR) L"Error", MB_OK | MB_ICONERROR);
}


void Logger::file(const std::string& msg)
{
    std::ofstream file;
    time_t currentTime;
    time(&currentTime);
    char timestamp[100];

    file.open (m_fileName.c_str(), std::ofstream::app);
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
