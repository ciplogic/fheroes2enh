//
// Created by ciprian on 11/28/17.
//

#include "FileUtils.h"

#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#include <bits/stat.h>

#else
#endif


#ifdef WIN32
#define stat _stat
#endif

namespace FileUtils
{
    long GetFileTime(std::string filename)
    {
        struct stat result;
        if (stat(filename.c_str(), &result) == 0)
        {
            return result.st_mtim.tv_sec;
        }
    }
}