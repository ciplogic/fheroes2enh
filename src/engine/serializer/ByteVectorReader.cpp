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
	std::vector<u8> result;
	result.reserve(sizeblock);
	for (int i = 0; i<sizeblock; i++)
	{
		result.push_back(get());
	}
	return result;
}

std::string ByteVectorReader::toString(int sizeBlock)
{
	auto raw = getRaw(sizeBlock);
	std::string text = reinterpret_cast<char*>(raw.data());
	return text;
}

ByteVectorReader & operator>>(ByteVectorReader & msg, u32 & val)
{
	val = msg.getLE32();
	return msg;
}

ByteVectorReader & operator>>(ByteVectorReader & msg, bool &v)
{
	v = msg.Get8();
	return msg;
}

ByteVectorReader & operator>>(ByteVectorReader & msg, std::string &v)
{
	u32 size = msg.getLE32();
	v.resize(size);

	for (char &it : v)
		it = msg.Get8();

	return msg;
}

ByteVectorReader & operator>>(ByteVectorReader &msg, s16 &val)
{
	val = msg.getLE16();
	return msg;
}
ByteVectorReader & operator>>(ByteVectorReader &msg, s32 &val)
{
	val = msg.getLE32();
	return msg;
}
ByteVectorReader & operator>>(ByteVectorReader & msg, Point &v)
{
	return msg >> v.x >> v.y;
}