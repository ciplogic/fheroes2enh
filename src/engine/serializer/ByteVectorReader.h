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

	void skip(u32 sz);
	u32 Get8();

	u32 getLE16();
	
	u32 getLE32();

	u16 get16();
	u32 get32();
	

	u32 getBE16();

	u32 getBE32();
	

	u32 size() const;
	void seek(u32 pos);
	u32 get();
	u32 tell() const;

	std::vector<u8> getRaw(size_t sizeblock);	

	std::string toString(int sizeBlock);

	void setBigEndian(int value);
	ByteVectorReader& operator>>(Size& v);



	ByteVectorReader &operator>>(float &v);

	template<class Type>
	void readToVec(std::vector<Type> &v)
	{
		const u32 size = get32();
		v.resize(size);
		for (auto& it : v)
			it.ReadFrom(*this);
	}

	template<class Type>
	ByteVectorReader &operator>>(std::vector<Type> &v)
	{
		const u32 size = get32();
		v.resize(size);
		for (auto& it : v)
			*this >> it;
		return *this;
	}

	template<class Type>
	ByteVectorReader &operator>>(std::list<Type> &v)
	{
		const u32 size = get32();
		v.resize(size);
		for (auto& it : v)
			*this >> it;
		return *this;
	}
	template<class Type1, class Type2>
	ByteVectorReader &operator>>(std::pair<Type1, Type2> &p)
	{
		return *this >> p.first >> p.second;
	}

	template<class Type1, class Type2>
	ByteVectorReader &operator>>(std::map<Type1, Type2> &v)
	{
		const u32 size = get32();
		v.clear();
		for (u32 ii = 0; ii < size; ++ii)
		{
			std::pair<Type1, Type2> pr;
			*this >> pr;
			v.insert(pr);
		}
		return *this;
	}

};

ByteVectorReader & operator>>(ByteVectorReader &msg, u8 &val);
ByteVectorReader & operator>>(ByteVectorReader &msg, s8 &val);
ByteVectorReader & operator>>(ByteVectorReader &msg, char &val);

ByteVectorReader & operator>>(ByteVectorReader &msg, u16 &val);
ByteVectorReader & operator>>(ByteVectorReader &msg, s16 &val);

ByteVectorReader &operator>>(ByteVectorReader &, u32 &);
ByteVectorReader &operator>>(ByteVectorReader &, s32 &);

ByteVectorReader & operator>>(ByteVectorReader & msg, bool &);
ByteVectorReader & operator>>(ByteVectorReader & msg, std::string &v);
ByteVectorReader & operator>>(ByteVectorReader & msg, Point &v);
