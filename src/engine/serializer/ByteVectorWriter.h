#ifndef ByteVectorWriter_H
#define ByteVectorWriter_H

#include "types.h"
#include <vector>

class ByteVectorWriter
{
    std::vector<u8> _data;
    int _pos;

    void put8(u8);


public:
    explicit ByteVectorWriter(int sz);
    void putLE16(u16 v);
    void putLE32(uint32_t v);

    const std::vector<u8>::const_iterator& data() const
    {
        return _data.begin();
    }
};

#endif
