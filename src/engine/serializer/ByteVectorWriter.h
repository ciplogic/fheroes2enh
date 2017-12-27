#ifndef ByteVectorWriter_H
#define ByteVectorWriter_H

#include "types.h"
#include <vector>

class ByteVectorWriter
{
    std::vector<u8> _data;
    void Add8(u8); 
    void Add16(u16);
public:

    ByteVectorWriter& Add(u8);
    ByteVectorWriter& Add(u16);
    ByteVectorWriter& Add(u32);
    ByteVectorWriter& Add(s32);
    std::vector<u8>& data();
};

#endif
