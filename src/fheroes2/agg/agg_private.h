//
// Created by ciprian on 4/27/19.
//

#pragma once

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <vector>
#include "settings.h"
#include "sprite.h"

namespace AGG
{
    class FAT
    {
    public:
        FAT() : crc(0), offset(0), size(0)
        {
        }

        uint32_t crc;
        uint32_t offset;
        uint32_t size;

        string Info() const;
    };


    class File
    {
    public:
        File();

        ~File();

        bool Open(const string&);

        bool isGood() const;

        const string& Name() const;

        const FAT& Fat(const string& key);

        vector<u8> Read(const string& str);

    private:
        string filename;
        unordered_map<string, FAT> fat;
        uint32_t count_items = 0;
        up<ByteVectorReader> stream;
        vector<u8> fileContent;
        string key;
        vector<u8> body;
    };

    struct icn_cache_t
    {
        icn_cache_t() : sprites(nullptr), reflect(nullptr), count(0)
        {
        }

        Sprite* sprites;
        Sprite* reflect;
        uint32_t count;
    };

    struct til_cache_t
    {
        til_cache_t() : sprites(nullptr), count(0)
        {
        }

        Surface* sprites;
        uint32_t count;
    };

    struct fnt_cache_t
    {
        Surface sfs[6]; /* small_white, small_yellow, medium_white, medium_yellow */
    };

    struct loop_sound_t
    {
        loop_sound_t(int w, int c) : sound(w), channel(c)
        {
        }

        bool operator==(int m82) const
        {
            return m82 == sound;
        }

        int sound;
        int channel;
    };
}