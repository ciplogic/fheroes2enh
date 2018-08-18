//
// Created by ciprian on 11/28/17.
//

#pragma once

#include <string>
#include <cstring>

namespace FileUtils
{
    template <class T>
    void ZeroMem(T& t)
    {
        memset(&t, 0, sizeof(T));
    }

    long GetFileTime(const std::string& filename);
}
