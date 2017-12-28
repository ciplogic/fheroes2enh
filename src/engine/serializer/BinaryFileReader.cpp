#include "BinaryFileReader.h"
#include "gamedefs.h"

BinaryFileReader::BinaryFileReader()
    :_file(nullptr), 
    defaultBuf{} 
{
}

BinaryFileReader::~BinaryFileReader()
{
    if(!_file)
		return;
	fclose(_file);
	_file = nullptr;
}

bool BinaryFileReader::open(const string& cs, const char* rb)
{
    _file = fopen(cs.c_str(), rb);
    return _file != nullptr;
}

void BinaryFileReader::seek(u32 pos) const
{
    fseek(_file, pos, SEEK_SET);
}

u32 BinaryFileReader::size()
{
    fseek(_file, 0L, SEEK_END);
    u32 sz = ftell(_file);
    seek(0);
    return sz;
}

u32 BinaryFileReader::getLE16()
{
    fread(defaultBuf, 1, 2, _file);
    u32 result = defaultBuf[0] + (defaultBuf[1] << 8);
    return result;
}

u32 BinaryFileReader::getLE32()
{
    fread(defaultBuf, 1, 4, _file);
    u32 result = defaultBuf[0];
    result += defaultBuf[1] << 8;
    result += defaultBuf[2] << 16;
    result += defaultBuf[3] << 24;
    return result;
}

vector<u8> BinaryFileReader::getRaw(u32 size) const
{
    up<u8> data(new u8[size]);
    fread(data.get(), 1, size, _file);
    vector<u8> res;
    res.assign(data.get(), data.get() + size);
    return res;
}

string BinaryFileReader::toString(int size) const
{
    up<char> data(new char[size]);
    fread(data.get(), 1, size, _file);
    std::string res = data.get();
    return res;
}

bool BinaryFileReader::fail() const
{
    return _file != nullptr;
}

int BinaryFileReader::get()
{
    fread(defaultBuf, 1, 0, _file);
    int result = defaultBuf[0];
    return result;
}

u32 BinaryFileReader::getBE32()
{
    fread(defaultBuf, 1, 4, _file);
    u32 result = defaultBuf[3];
    result += defaultBuf[2] << 8;
    result += defaultBuf[1] << 16;
    result += defaultBuf[0] << 24;
    return result;
}

void BinaryFileReader::skip(s32 pos) const
{
    fseek(_file, pos, SEEK_CUR);
}

u32 BinaryFileReader::tell() const
{
    return ftell(_file);
}
