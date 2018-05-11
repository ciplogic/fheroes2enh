#include "BinaryFileReader.h"
#include "gamedefs.h"
#include <fstream>

BinaryFileReader::BinaryFileReader()
        : _file(nullptr),
          defaultBuf{}
{
}

BinaryFileReader::~BinaryFileReader()
{
    close();
}

bool BinaryFileReader::open(const std::string &cs, const char *rb)
{
    _file = fopen(cs.c_str(), rb);
    return _file != nullptr;
}

void BinaryFileReader::seek(uint32_t pos) const
{
    fseek(_file, pos, SEEK_SET);
}

uint32_t BinaryFileReader::size()
{
    fseek(_file, 0L, SEEK_END);
    uint32_t sz = ftell(_file);
    seek(0);
    return sz;
}

uint32_t BinaryFileReader::getLE16()
{
    fread(defaultBuf, 1, 2, _file);
    uint32_t result = defaultBuf[0] + (defaultBuf[1] << 8);
    return result;
}

uint32_t BinaryFileReader::getLE32()
{
    fread(defaultBuf, 1, 4, _file);
    uint32_t result = defaultBuf[0];
    result += defaultBuf[1] << 8;
    result += defaultBuf[2] << 16;
    result += defaultBuf[3] << 24;
    return result;
}

std::vector<u8> BinaryFileReader::getRaw(uint32_t size) const
{
    up<u8> data(new u8[size]);
    fread(data.get(), 1, size, _file);
    std::vector<u8> res;
    res.assign(data.get(), data.get() + size);
    return res;
}

std::string BinaryFileReader::toString(int size) const
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

uint32_t BinaryFileReader::getBE32()
{
    fread(defaultBuf, 1, 4, _file);
    uint32_t result = defaultBuf[3];
    result += defaultBuf[2] << 8;
    result += defaultBuf[1] << 16;
    result += defaultBuf[0] << 24;
    return result;
}

void BinaryFileReader::skip(s32 pos) const
{
    fseek(_file, pos, SEEK_CUR);
}

uint32_t BinaryFileReader::tell() const
{
    return ftell(_file);
}

void BinaryFileReader::close()
{
    if (!_file)
        return;
    fclose(_file);
    _file = nullptr;
}

namespace FileUtils
{
    std::vector<u8> readFileBytes(std::string fileName)
    {
        std::vector<u8> result;
        BinaryFileReader reader;
        if (!reader.open(fileName, "rb"))
        {
            return result;
        }
        int fileSize = reader.size();
        reader.seek(0);
        result = reader.getRaw(fileSize);
        return result;
    }
    bool Exists(std::string fileName)
    {
        std::ifstream infile(fileName);
        return infile.is_open();
    }
    std::vector<std::string> readFileLines(std::string fileName)
    {
        std::vector<std::string> result;
        std::string line;
        std::ifstream infile(fileName);
        if (!infile.is_open())
            return result;
        while (std::getline(infile, line))
        {
            result.push_back(line);
        }
        return result;
    }
    void writeFileBytes(std::string fileName, const std::vector<u8>& v)
    {
        std::ofstream outfile(fileName, std::ios::out | std::ios::binary);
        outfile.write((const char*)&v[0], v.size());
    }

    void writeFileString(std::string fileName, std::string text)
    {
        std::ofstream outfile(fileName);
        outfile << text;
    }
}
