//
// Created by ciprian on 11/28/17.
//

#include "FileUtils.h"

#include <sys/stat.h>

#ifndef WIN32

#include <sys/types.h>
#include <unistd.h>

#else

#define stat _stat
#endif

#include <cstdlib>

namespace FileUtils
{
    long GetFileTime(const std::string& filename)
    {
        struct stat result{};
        if (stat(filename.c_str(), &result) != 0)
            return 0;
#ifdef __APPLE__
        return result.st_mtimespec.tv_sec;
#elif WIN32
        return result.st_mtime;
#else
        return result.st_mtim.tv_sec;
#endif
    }
}
