/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#pragma once

#include "serialize.h"
#include "ByteVectorWriter.h"
#include "ByteVectorReader.h"
#include "rect.h"

struct cost_t
{
    u16 gold;
    u8 wood;
    u8 mercury;
    u8 ore;
    u8 sulfur;
    u8 crystal;
    u8 gems;
};

#define COST_NONE { 0, 0, 0, 0, 0, 0 ,0 }

class ResourceCount;

namespace Resource
{
    enum
    {
        UNKNOWN = 0x00,
        WOOD = 0x01,
        MERCURY = 0x02,
        ORE = 0x04,
        SULFUR = 0x08,
        CRYSTAL = 0x10,
        GEMS = 0x20,
        GOLD = 0x40,
        ALL = WOOD | MERCURY | ORE | SULFUR | CRYSTAL | GEMS | GOLD
    };
}

class Funds
{
public:
    Funds();

    Funds(s32 _ore, s32 _wood, s32 _mercury, s32 _sulfur, s32 _crystal, s32 _gems, s32 _gold);

    Funds(int rs, uint32_t count);

    Funds(const cost_t &);

    Funds(const ResourceCount &);

    Funds operator+(const Funds &) const;

    Funds operator*(uint32_t mul) const;

    Funds operator-(const Funds &) const;

    Funds &operator+=(const Funds &);

    Funds &operator*=(uint32_t mul);

    Funds &operator-=(const Funds &);

    Funds &operator=(const cost_t &);

    s32 Get(int rs) const;

    s32 *GetPtr(int rs);

    bool operator<(const Funds &) const;

    bool operator<=(const Funds &) const;

    bool operator>(const Funds &) const;

    bool operator>=(const Funds &) const;

    int GetValidItems() const;

    uint32_t GetValidItemsCount() const;

    void Reset();

    string String() const;

    s32 wood;
    s32 mercury;
    s32 ore;
    s32 sulfur;
    s32 crystal;
    s32 gems;
    s32 gold;
};

StreamBase &operator<<(StreamBase &, const cost_t &);

void serializeTo(ByteVectorWriter &msg, cost_t &res);

StreamBase &operator<<(StreamBase &, const Funds &);

void serializeTo(ByteVectorWriter &msg, Funds &res);

ByteVectorReader &operator>>(ByteVectorReader &, cost_t &);

ByteVectorReader &operator>>(ByteVectorReader &, Funds &);

namespace Resource
{
    std::string String(int resource);

    int Rand(bool with_gold = false);

    /* return index sprite objnrsrc.icn */
    uint32_t GetIndexSprite(int resource);

    int FromIndexSprite(uint32_t index);

    /* return index sprite resource.icn */
    uint32_t GetIndexSprite2(int resource);

    int FromIndexSprite2(uint32_t index);

    class BoxSprite : protected Rect
    {
    public:
        BoxSprite(const Funds &, uint32_t);

        const Rect &GetArea() const;

        void SetPos(s32, s32);

        void Redraw() const;

        const Funds rs;
    };
}

