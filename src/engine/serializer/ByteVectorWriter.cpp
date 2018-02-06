#include "ByteVectorWriter.h"

void ByteVectorWriter::Add8(u8 val)
{
    this->_data.push_back(val);
}

void ByteVectorWriter::Add16(u16 val)
{
    Add8(val);
    val = val >> 8;
    Add8(val);
}

ByteVectorWriter &ByteVectorWriter::Add(u8 value)
{
    Add8(value);
    return *this;
}

ByteVectorWriter &ByteVectorWriter::Add(u16 value)
{
    Add16(value);
    return *this;
}

ByteVectorWriter &ByteVectorWriter::Add(u32 value)
{
    Add16(value);
    Add16(value >> 16);
    return *this;
}

ByteVectorWriter &ByteVectorWriter::Add(s32 val)
{
    u32 uVal = val;
    Add(uVal);
    return *this;
}

std::vector<u8> &ByteVectorWriter::data()
{
    return _data;
}

