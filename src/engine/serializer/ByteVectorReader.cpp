#include "ByteVectorReader.h"
#include "rect.h"

ByteVectorReader::ByteVectorReader(const std::vector<u8>& data)
	: _data(data), _pos(0)
{
}

void ByteVectorReader::skip(u32 sz)
{
	_pos += sz;
}

u32 ByteVectorReader::Get8()
{
	const u32 result = _data[_pos];
	_pos++;
	return result;
}

u32 ByteVectorReader::getLE16()
{
	u32 lo = Get8();
	u32 hi = Get8();
	return lo + (hi << 8);
}

u32 ByteVectorReader::getLE32()
{
	u32 lo = getLE16();
	u32 hi = getLE16();
	return lo + (hi << 16);
}

u16 ByteVectorReader::get16()
{
	u16 value = _isBigEndian ? getBE16() : getLE16();
	return value;
}

u32 ByteVectorReader::get32()
{
	u32 value = _isBigEndian?getBE32():getLE32();
	return value;
}

u32 ByteVectorReader::getBE16()
{
	u32 lo = Get8();
	u32 hi = Get8();
	return hi + (lo << 8);
}

u32 ByteVectorReader::getBE32()
{
	u32 lo = getBE16();
	u32 hi = getBE16();
	return hi + (lo << 16);
}

u32 ByteVectorReader::size() const
{
	return _data.size();
}

void ByteVectorReader::seek(u32 pos)
{
	_pos = pos;
}

u32 ByteVectorReader::get()
{
	return Get8();
}

u32 ByteVectorReader::tell() const
{
	return _pos;
}

std::vector<u8> ByteVectorReader::getRaw(size_t sizeblock)
{
	if (sizeblock == 0)
	{
		sizeblock = size() - tell();
	}
	auto start = _data.begin()+_pos;
	auto endPos = start + sizeblock;
		
	std::vector<u8> result;
	result.assign(start, endPos);
	_pos += sizeblock;
	return result;
}

std::string ByteVectorReader::toString(int sizeBlock)
{
	auto raw = getRaw(sizeBlock);
	std::string text = reinterpret_cast<char*>(raw.data());
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

ByteVectorReader& ByteVectorReader::operator>>(float& v)
{
	s32 intpart;
	s32 decpart;
	*this >> intpart >> decpart;
	v = intpart + decpart / 100000000.0;
	return *this;
}

ByteVectorReader & operator>>(ByteVectorReader & msg, u32 & val)
{
	val = msg.get32();
	return msg;
}

ByteVectorReader & operator>>(ByteVectorReader & msg, bool &v)
{
	v = msg.Get8();
	return msg;
}

ByteVectorReader & operator>>(ByteVectorReader & msg, std::string &v)
{
	u32 size = msg.get32();
	v.resize(size);

	for (char &it : v)
		it = msg.Get8();

	return msg;
}

ByteVectorReader & operator>>(ByteVectorReader &msg, u8 &val)
{
	val = msg.Get8();
	return msg;
}

ByteVectorReader & operator>>(ByteVectorReader &msg, s8 &val)
{
	val = msg.Get8();
	return msg;
}
ByteVectorReader & operator>>(ByteVectorReader &msg, char &val)
{
	val = msg.Get8();
	return msg;
}
ByteVectorReader & operator>>(ByteVectorReader &msg, u16 &val)
{
	val = msg.get16();
	return msg;
}
ByteVectorReader & operator>>(ByteVectorReader &msg, s16 &val)
{
	val = msg.get16();
	return msg;
}
ByteVectorReader & operator>>(ByteVectorReader &msg, s32 &val)
{
	val = msg.get32();
	return msg;
}
ByteVectorReader & operator>>(ByteVectorReader & msg, Point &v)
{
	return msg >> v.x >> v.y;
}