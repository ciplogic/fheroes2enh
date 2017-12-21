#ifndef BinaryFileReader_H
#define BinaryFileReader_H

#include "types.h"
#include <vector>
#include <string>

class BinaryFileReader
{
    FILE* _file;
    u8 defaultBuf[4];
public:
    BinaryFileReader();
    ~BinaryFileReader();
    bool open(const std::string& cs, const char* rb);
    void seek(u32 pos);
    u32 size();
    u32 getLE16();
    u32 getLE32();
    std::vector<u8> getRaw(u32 size) const;
    
    std::string toString(int size);
    bool fail() const;
    int get();
    u32 getBE32();
    void skip(s32 pos);
    u32 tell();
};

#endif
