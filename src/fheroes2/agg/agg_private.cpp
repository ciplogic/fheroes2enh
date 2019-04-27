//
// Created by ciprian on 4/27/19.
//

#include "agg_private.h"

#include "BinaryFileReader.h"
#include <sstream>

namespace
{
    const int FATSIZENAME = 15;
}

/*AGG::File constructor */
AGG::File::File() : count_items(0)
{
}

bool AGG::File::Open(const string& fname)
{
    filename = fname;
    if (!FileUtils::Exists(fname))
        return false;
    fileContent = FileUtils::readFileBytes(filename);
    stream = std::make_unique<ByteVectorReader>(fileContent);

    const uint32_t size = stream->size();

    count_items = stream->getLE16();

    stream->seek(size - FATSIZENAME * count_items);
    std::vector<std::string> vectorNames;
    vectorNames.reserve(count_items);
    for (uint32_t ii = 0; ii < count_items; ++ii)
    {
        vectorNames.push_back(stream->toString(FATSIZENAME));
    }
    stream->seek(2);
    for (uint32_t ii = 0; ii < count_items; ++ii)
    {
        const string& itemName = vectorNames[ii];
        FAT f;
        const auto crc = stream->getLE32();
        f.crc = crc;
        const auto offset = stream->getLE32();
        f.offset = offset;
        const auto sizeChunk = stream->getLE32();
        f.size = sizeChunk;
        fat[itemName] = f;
    }

    return true;
}

AGG::File::~File() = default;

bool AGG::File::isGood() const
{
    return stream && stream->size() && count_items;
}

/* get AGG file name */
const string& AGG::File::Name() const
{
    return filename;
}

/* get FAT element */
const AGG::FAT& AGG::File::Fat(const string& key)
{
    return fat[key];
}

/* dump FAT */
string AGG::FAT::Info() const
{
    ostringstream os;

    os << "crc: " << crc << ", offset: " << offset << ", size: " << size;
    return os.str();
}

/* read element to body */
vector<u8> AGG::File::Read(const string& str)
{
    const auto it = fat.find(str);

    if (it == fat.end())
    {
        vector<u8> emptyBuf;
        return emptyBuf;
    }

    const FAT& f = (*it).second;
    key = str;

    if (!f.size)
    {
        vector<u8> emptyBuf;
        return emptyBuf;
    }
    stream->seek(f.offset);
    vector<u8> body = stream->getRaw(f.size);


    return body;
}

