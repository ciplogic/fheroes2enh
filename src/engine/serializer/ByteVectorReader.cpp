#include "ByteVectorReader.h"
#include "rect.h"

namespace
{
    namespace Endian
    {
        constexpr uint32_t uint32_ = 0x01020304;
        constexpr uint8_t magic_ = static_cast<const uint8_t&>(uint32_);
        const bool isLittle = magic_ == 0x04;
    }
}

ByteVectorReader::ByteVectorReader(const std::vector<u8>& data)
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
    if (!Endian::isLittle)
    {
        uint32_t lo = Get8();
        uint32_t hi = Get8();
        return lo + (hi << 8);
    }
    auto* resultPtr = reinterpret_cast<const uint16_t *>(_data.data() + _pos);
    _pos += 2;
    const uint16_t result = *resultPtr;
    return result;
}

uint32_t ByteVectorReader::getLE32()
{
    if (!Endian::isLittle)
    {
        auto llo = Get8();
        auto lhi = Get8();
        auto hlo = Get8();
        auto hhi = Get8();
        uint32_t lo = llo + (lhi << 8);
        uint32_t hi = (hlo << 16) + (hhi << 24);
        uint32_t result = lo + hi;
        return result;
    }
    auto* resultPtr = reinterpret_cast<const uint32_t *>(_data.data() + _pos);
    _pos += 4;
    const auto result = *resultPtr;
    return result;
}

u16 ByteVectorReader::get16()
{
    const u16 value = _isBigEndian ? getBE16() : getLE16();
    return value;
}

uint32_t ByteVectorReader::get32()
{
    const uint32_t value = _isBigEndian ? getBE32() : getLE32();
    return value;
}

uint32_t ByteVectorReader::getBE16()
{
    if (Endian::isLittle)
    {
        const uint32_t lo = Get8();
        const uint32_t hi = Get8();
        return hi + (lo << 8);
    }
    auto* resultPtr = reinterpret_cast<const uint16_t *>(_data.data() + _pos);
    _pos += 2;
    uint16_t result = *resultPtr;
    return result;
}

uint32_t ByteVectorReader::getBE32()
{
    if (Endian::isLittle)
    {
        const uint32_t lo1 = Get8();
        const uint32_t hi1 = Get8();
        const uint32_t lo2 = Get8();
        const uint32_t hi2 = Get8();
        const uint32_t lo = (hi1 << 16) + (lo1 << 24);
        const uint32_t hi = hi2 + (lo2 << 8);
        return hi + lo;
    }
    auto* resultPtr = reinterpret_cast<const uint32_t *>(_data.data() + _pos);
    _pos += 4;
    uint32_t result = *resultPtr;
    return result;
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
    const auto start = _data.begin() + _pos;
    const auto end_pos = start + sizeblock;

    std::vector<u8> result;
    result.assign(start, end_pos);
    _pos += sizeblock;
    return result;
}

std::string ByteVectorReader::toString(int sizeBlock)
{
    auto raw = getRaw(sizeBlock);
    raw.push_back(0);
    std::string text = reinterpret_cast<char *>(raw.data());
    return text;
}

void ByteVectorReader::setBigEndian(int value)
{
    _isBigEndian = value;
}


ByteVectorReader& ByteVectorReader::operator>>(Size& v)
{
    return *this >> v.w >> v.h;
}

ByteVectorReader& ByteVectorReader::operator>>(float& v)
{
    s32 intpart;
    s32 decpart;
    *this >> intpart >> decpart;
    v = intpart + decpart / 100000000.0;
    return *this;
}

ByteVectorReader& operator>>(ByteVectorReader& msg, uint32_t& val)
{
    val = msg.get32();
    return msg;
}

ByteVectorReader& operator>>(ByteVectorReader& msg, bool& v)
{
    v = msg.Get8();
    return msg;
}

ByteVectorReader& operator>>(ByteVectorReader& msg, std::string& v)
{
    v = msg.readString();
    return msg;
}

std::string ByteVectorReader::readString()
{
    const uint32_t size = get32();
    const auto* vData = reinterpret_cast<const char*>(_data.data() + _pos);
    std::string v(vData, size);
    _pos += size;
    return v;
}

ByteVectorReader& operator>>(ByteVectorReader& msg, u8& val)
{
    val = msg.Get8();
    return msg;
}

ByteVectorReader& operator>>(ByteVectorReader& msg, s8& val)
{
    val = msg.Get8();
    return msg;
}

ByteVectorReader& operator>>(ByteVectorReader& msg, char& val)
{
    val = msg.Get8();
    return msg;
}

ByteVectorReader& operator>>(ByteVectorReader& msg, u16& val)
{
    val = msg.get16();
    return msg;
}

ByteVectorReader& operator>>(ByteVectorReader& msg, s16& val)
{
    val = msg.get16();
    return msg;
}

ByteVectorReader& operator>>(ByteVectorReader& msg, s32& val)
{
    val = msg.get32();
    return msg;
}

ByteVectorReader& operator>>(ByteVectorReader& msg, Point& v)
{
    return msg >> v.x >> v.y;
}
