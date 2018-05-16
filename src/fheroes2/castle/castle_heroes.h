/***************************************************************************
 *   Copyright (C) 2011 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#ifndef H2CASTLE_HEROES_H
#define H2CASTLE_HEROES_H

#include <utility>
#include <algorithm>
#include "heroes.h"

class CastleHeroes : protected pair<sp<Heroes>, sp<Heroes>>
{
public:
    CastleHeroes(sp<Heroes> guest, sp<Heroes> guard) : pair<sp<Heroes>, sp<Heroes> >(guest, guard)
    {};

    sp<Heroes> Guest()
    { return first; };

    sp<Heroes> Guard()
    { return second; };

    const sp<Heroes> Guest() const
    { return first; };

    const sp<Heroes> Guard() const
    { return second; };

    sp<Heroes> GuestFirst()
    { return first ? first : second; };

    sp<Heroes> GuardFirst()
    { return second ? second : first; };

    bool operator==(const sp<Heroes> hero) const
    { return first == hero || second == hero; };

    void Swap()
    { std::swap(first, second); };

    bool FullHouse() const
    { return first && second; };

    bool IsValid() const
    { return first || second; };
};

#endif
