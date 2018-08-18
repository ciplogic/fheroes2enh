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

#include "gamedefs.h"
#include "ByteVectorReader.h"
#include "ByteVectorWriter.h"

#define MODES_ALL 0xFFFFFFFF


class BitModes
{
public:
    BitModes() : modes(0)
    {
    }

    virtual ~BitModes()
    = default;

    uint32_t operator()() const
    {
        return modes;
    }

    void SetModes(uint32_t f)
    {
        modes |= f;
    }

    void ResetModes(uint32_t f)
    {
        modes &= ~f;
    }

    void ToggleModes(uint32_t f)
    {
        modes ^= f;
    }

    bool Modes(uint32_t f) const
    {
        return (modes & f) != 0;
    }

protected:
    friend ByteVectorWriter& operator<<(ByteVectorWriter&, const BitModes&);

    friend ByteVectorReader& operator>>(ByteVectorReader&, BitModes&);

    uint32_t modes;
};

ByteVectorWriter& operator<<(ByteVectorWriter&, const BitModes&);

ByteVectorReader& operator>>(ByteVectorReader&, BitModes&);
