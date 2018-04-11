#include "ByteVectorWriter.h"

void ByteVectorWriter::put8(u8 val)
{
    _data[_pos] = val;
    _pos++;
}

ByteVectorWriter::ByteVectorWriter(int sz)
    : _pos(0)
{
    _data.resize(sz, 0);
}

void ByteVectorWriter::putLE16(u16 v)
{
    put8(v);
    put8(v >> 8);
}
void ByteVectorWriter::putLE32(u32 v)
{
    put8(v);
    put8(v >> 8);
    put8(v >> 16);
    put8(v >> 24);
}