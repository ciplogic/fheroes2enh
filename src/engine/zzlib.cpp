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

#include "system.h"
#include "zzlib.h"

bool ZStreamFile::read(const string &fn, size_t offset)
{
    StreamFile sf;
    sf.setbigendian(true);

    if (sf.open(fn, "rb"))
    {
        if (offset) sf.seek(offset);

        const u32 size0 = sf.get32(); // raw size
        vector<u8> raw = sf.getRaw(size0);
        putRaw(reinterpret_cast<const char *>(&raw[0]), raw.size());
        seek(0);
        return !fail();
    }
    return false;
}

bool ZStreamFile::write(const string &fn, bool append) const
{
    StreamFile sf;
    sf.setbigendian(true);

    if (sf.open(fn, append ? "ab" : "wb"))
    {
        sf.put32(size());
        sf.putRaw(reinterpret_cast<const char *>(data()), size());
        return !sf.fail();
    }
    return false;
}

