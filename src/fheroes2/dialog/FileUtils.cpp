//
// Created by ciprian on 11/28/17.
//

#include "FileUtils.h"

#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#else
#endif

#include <cstdlib>

#ifdef WIN32
#define stat _stat
#endif

namespace FileUtils
{
    long GetFileTime(std::string filename)
    {
        struct stat result{};
        if (stat(filename.c_str(), &result) != 0)
            return 0;
#ifndef __APPLE__
        return result.st_mtim.tv_sec;
#else
        return result.st_mtimespec.tv_sec;
#endif
    }
}