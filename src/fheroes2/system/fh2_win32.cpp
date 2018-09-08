#include <vector>
#include <string>

#ifdef WIN32
#include "dir.h"
#include <Windows.h>

std::vector<std::string> extractArgsVector(int argc, char** argv)
{
    int nArgs;
    std::wstring commandLine = GetCommandLineW();

    LPWSTR* szArglist = CommandLineToArgvW(commandLine.c_str(), &nArgs);

    std::vector<std::string> vArgv;
    vArgv.reserve(nArgs);
    for (int i = 0; i < nArgs; i++)
    {
        vArgv.emplace_back(ws2s(szArglist[i]));
    }
    LocalFree(szArglist);
    return vArgv;
}

#else


std::vector<std::string> extractArgsVector(int argc, char** argv)
{
    std::vector<std::string> vArgv;
    vArgv.reserve(argc);
    for (int i = 0; i < argc; i++)
    {
        vArgv.emplace_back(argv[i]);
    }
    return vArgv;
}
#endif
