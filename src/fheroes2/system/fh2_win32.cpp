#include <Windows.h>
#include <vector>
#include "dir.h"

#ifdef WIN32

std::vector<std::string> extractArgsVector(int argc, char** argv)
{
	std::vector<std::string> vArgv;

	int nArgs;
	std::wstring commandLine = GetCommandLineW();

	LPWSTR* szArglist = CommandLineToArgvW(commandLine.c_str(), &nArgs);
	for (int i = 0; i < nArgs; i++)
	{
		vArgv.emplace_back(ws2s(szArglist[i]));
	}
	LocalFree(szArglist);
	return vArgv;
}

#endif
