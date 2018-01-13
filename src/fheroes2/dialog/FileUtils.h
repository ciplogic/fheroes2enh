//
// Created by ciprian on 11/28/17.
//

#ifndef FHEROES2_FILEUTILS_H
#define FHEROES2_FILEUTILS_H


#include <string>
#include <cstring>

namespace FileUtils
{    template <class T> void ZeroMem(T& t){
        memset(&t, 0, sizeof(T));
    }

    long GetFileTime(std::string filename);
}


#endif //FHEROES2_FILEUTILS_H
