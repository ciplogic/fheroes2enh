#ifndef ByteVectorReader_H
#define ByteVectorReader_H

#include "types.h"
#include <list>
#include <vector>
#include <string>

struct Point;

class ByteVectorReader
{
	const std::vector<u8>& _data;
	int _pos;
public:
	explicit ByteVectorReader(const std::vector<u8>& data);

	void skip(u32 sz);
	u32 Get8();

	u32 getLE16();
	
	u32 getLE32();
	

	u32 getBE16();

	u32 getBE32();
	

	u32 size() const;
	void seek(u32 pos);
	u32 get();
	u32 tell() const;

	std::vector<u8> getRaw(size_t sizeblock);	

	std::string toString(int sizeBlock);

	template<class Type>
	void readToVec(std::vector<Type> &v)
	{
		const u32 size = getLE32();
		v.resize(size);
		for (auto& it : v)
			it.ReadFrom(*this);
	}

	template<class Type>
	ByteVectorReader &operator>>(std::vector<Type> &v)
	{
		const u32 size = getLE32();
		v.resize(size);
		for (auto& it : v)
			*this >> it;
		return *this;
	}

	template<class Type>
	ByteVectorReader &operator>>(std::list<Type> &v)
	{
		const u32 size = getLE32();
		v.resize(size);
		for (auto& it : v)
			*this >> it;
		return *this;
	}

};



ByteVectorReader & operator>>(ByteVectorReader &msg, s16 &val);

ByteVectorReader &operator>>(ByteVectorReader &, u32 &);
ByteVectorReader &operator>>(ByteVectorReader &, s32 &);

ByteVectorReader & operator>>(ByteVectorReader & msg, bool &);
ByteVectorReader & operator>>(ByteVectorReader & msg, std::string &v);
ByteVectorReader & operator>>(ByteVectorReader & msg, Point &v);

#endif
