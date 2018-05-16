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

#include "world.h"

Recruits::Recruits() : pair<int, int>(Heroes::UNKNOWN, Heroes::UNKNOWN)
{
}

void Recruits::Reset()
{
    first = Heroes::UNKNOWN;
    second = Heroes::UNKNOWN;
}

int Recruits::GetID1() const
{
    return first;
}

int Recruits::GetID2() const
{
    return second;
}

const sp<Heroes> Recruits::GetHero1() const
{
    return world.GetHeroes(first);
}

const sp<Heroes> Recruits::GetHero2() const
{
    return world.GetHeroes(second);
}

sp<Heroes> Recruits::GetHero1()
{
    return world.GetHeroes(first);
}

sp<Heroes> Recruits::GetHero2()
{
    return world.GetHeroes(second);
}

void Recruits::SetHero1(const sp<Heroes> hero)
{
    first = hero ? hero->hid : Heroes::UNKNOWN;
}

void Recruits::SetHero2(const sp<Heroes> hero)
{
    second = hero ? hero->hid : Heroes::UNKNOWN;
}

ByteVectorReader &operator>>(ByteVectorReader &sb, Recruits &rt)
{
    return sb >> rt.first >> rt.second;
}
