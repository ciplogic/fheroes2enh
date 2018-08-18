#pragma once

#include "types.h"
#include <list>
#include <vector>
#include <string>
#include "rect.h"
#include <map>

struct Point;

class ByteVectorReader
{
    const std::vector<u8>& _data;
    int _pos;
    bool _isBigEndian = false;

public:
    explicit ByteVectorReader(const std::vector<u8>& data);

    void skip(uint32_t sz);

    uint32_t Get8();

    uint32_t getLE16();

    uint32_t getLE32();

    u16 get16();

    uint32_t get32();


    uint32_t getBE16();

    uint32_t getBE32();


    uint32_t size() const;

    void seek(uint32_t pos);

    uint32_t get();

    uint32_t tell() const;

    std::vector<u8> getRaw(size_t sizeblock);

    std::string toString(int sizeBlock = 0);

    void setBigEndian(int value);

    ByteVectorReader& operator>>(Size& v);


    ByteVectorReader& operator>>(float& v);

    template <class Type>
    void readToVec(std::vector<Type>& v)
    {
        const uint32_t size = get32();
        v.resize(size);
        for (auto& it : v)
            it.ReadFrom(*this);
    }

    template <class Type>
    ByteVectorReader& operator>>(std::vector<Type>& v)
    {
        const uint32_t size = get32();
        v.resize(size);
        for (auto& it : v)
            *this >> it;
        return *this;
    }

    template <class Type>
    ByteVectorReader& operator>>(std::list<Type>& v)
    {
        const uint32_t size = get32();
        v.resize(size);
        for (auto& it : v)
            *this >> it;
        return *this;
    }

    template <class Type1, class Type2>
    ByteVectorReader& operator>>(std::pair<Type1, Type2>& p)
    {
        return *this >> p.first >> p.second;
    }

    template <class Type1, class Type2>
    ByteVectorReader& operator>>(std::map<Type1, Type2>& v)
    {
        const uint32_t size = get32();
        v.clear();
        for (uint32_t ii = 0; ii < size; ++ii)
        {
            std::pair<Type1, Type2> pr;
            *this >> pr;
            v.insert(pr);
        }
        return *this;
    }

    std::string readString();
};

ByteVectorReader& operator>>(ByteVectorReader& msg, u8& val);

ByteVectorReader& operator>>(ByteVectorReader& msg, s8& val);

ByteVectorReader& operator>>(ByteVectorReader& msg, char& val);

ByteVectorReader& operator>>(ByteVectorReader& msg, u16& val);

ByteVectorReader& operator>>(ByteVectorReader& msg, s16& val);

ByteVectorReader& operator>>(ByteVectorReader&, uint32_t&);

ByteVectorReader& operator>>(ByteVectorReader&, s32&);

ByteVectorReader& operator>>(ByteVectorReader& msg, bool&);

ByteVectorReader& operator>>(ByteVectorReader& msg, std::string& v);

ByteVectorReader& operator>>(ByteVectorReader& msg, Point& v);
