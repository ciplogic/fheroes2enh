#include "ByteVectorReader.h"
#include "rect.h"

ByteVectorReader::ByteVectorReader(const std::vector<u8> &data)
        : _data(data), _pos(0)
{
}

void ByteVectorReader::skip(uint32_t sz)
{
    _pos += sz;
}

uint32_t ByteVectorReader::Get8()
{
    const uint32_t result = _data[_pos];
    _pos++;
    return result;
}

uint32_t ByteVectorReader::getLE16()
{
    uint32_t lo = Get8();
    uint32_t hi = Get8();
    return lo + (hi << 8);
}

uint32_t ByteVectorReader::getLE32()
{
    uint32_t lo = getLE16();
    uint32_t hi = getLE16();
    return lo + (hi << 16);
}

u16 ByteVectorReader::get16()
{
    u16 value = _isBigEndian ? getBE16() : getLE16();
    return value;
}

uint32_t ByteVectorReader::get32()
{
    uint32_t value = _isBigEndian ? getBE32() : getLE32();
    return value;
}

uint32_t ByteVectorReader::getBE16()
{
    uint32_t lo = Get8();
    uint32_t hi = Get8();
    return hi + (lo << 8);
}

uint32_t ByteVectorReader::getBE32()
{
    uint32_t lo1 = Get8();
    uint32_t hi1 = Get8();
    uint32_t lo2 = Get8();
    uint32_t hi2 = Get8();
    uint32_t lo = hi1 + (lo1 << 8);
    uint32_t hi = hi2 + (lo2 << 8);
    return hi + (lo << 16);
}

uint32_t ByteVectorReader::size() const
{
    return _data.size();
}

void ByteVectorReader::seek(uint32_t pos)
{
    _pos = pos;
}

uint32_t ByteVectorReader::get()
{
    return Get8();
}

uint32_t ByteVectorReader::tell() const
{
    return _pos;
}

std::vector<u8> ByteVectorReader::getRaw(size_t sizeblock)
{
    if (sizeblock == 0)
    {
        sizeblock = size() - tell();
    }
    auto start = _data.begin() + _pos;
    auto endPos = start + sizeblock;

    std::vector<u8> result;
    result.assign(start, endPos);
    _pos += sizeblock;
    return result;
}

std::string ByteVectorReader::toString(int sizeBlock)
{
    auto raw = getRaw(sizeBlock);
    std::string text = reinterpret_cast<char *>(raw.data());
    return text;
}

void ByteVectorReader::setBigEndian(int value)
{
    _isBigEndian = value;
}


ByteVectorReader &ByteVectorReader::operator>>(Size &v)
{
    return *this >> v.w >> v.h;
}

ByteVectorReader &ByteVectorReader::operator>>(float &v)
{
    s32 intpart;
    s32 decpart;
    *this >> intpart >> decpart;
    v = intpart + decpart / 100000000.0;
    return *this;
}

ByteVectorReader &operator>>(ByteVectorReader &msg, uint32_t &val)
{
    val = msg.get32();
    return msg;
}

ByteVectorReader &operator>>(ByteVectorReader &msg, bool &v)
{
    v = msg.Get8();
    return msg;
}

ByteVectorReader &operator>>(ByteVectorReader &msg, std::string &v)
{
    uint32_t size = msg.get32();
    v.resize(size);

    for (char &it : v)
        it = msg.Get8();

    return msg;
}

ByteVectorReader &operator>>(ByteVectorReader &msg, u8 &val)
{
    val = msg.Get8();
    return msg;
}

ByteVectorReader &operator>>(ByteVectorReader &msg, s8 &val)
{
    val = msg.Get8();
    return msg;
}

ByteVectorReader &operator>>(ByteVectorReader &msg, char &val)
{
    val = msg.Get8();
    return msg;
}

ByteVectorReader &operator>>(ByteVectorReader &msg, u16 &val)
{
    val = msg.get16();
    return msg;
}

ByteVectorReader &operator>>(ByteVectorReader &msg, s16 &val)
{
    val = msg.get16();
    return msg;
}

ByteVectorReader &operator>>(ByteVectorReader &msg, s32 &val)
{
    val = msg.get32();
    return msg;
}

ByteVectorReader &operator>>(ByteVectorReader &msg, Point &v)
{
    return msg >> v.x >> v.y;
}