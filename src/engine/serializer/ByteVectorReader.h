#ifndef ByteVectorReader_H
#define ByteVectorReader_H

#include "types.h"
#include <vector>
#include <string>

class ByteVectorReader
{
	const std::vector<u8>& _data;
	int _pos;
public:
	ByteVectorReader(const std::vector<u8>& data);

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
};

#endif
