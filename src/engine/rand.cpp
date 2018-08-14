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

#include <cstdlib>
#include <ctime>

#include "system.h"
#include "rand.h"
#include <sstream>
#include <iostream>

void Rand::Init()
{
    srand(static_cast<uint32_t>(time(nullptr)));
}

int32_t Rand::Get(int32_t min, int32_t max)
{
    if (max)
    {
        if (min > max) swap(min, max);

        return min + Get(max - min);
    }

    return static_cast<uint32_t>((min + 1) * (rand() / (RAND_MAX + 1.0)));
}

Rand::Queue::Queue(uint32_t size)
{
    reserve(size);
}

void Rand::Queue::Reset()
{
    clear();
}

void Rand::Queue::Push(s32 value, uint32_t percent)
{
    if (percent)
        push_back(make_pair(value, percent));
}

size_t Rand::Queue::Size() const
{
    return size();
}

s32 Rand::Queue::Get()
{
    // get max

    uint32_t max = 0;
    for (auto &it : *this)
        max += it.second;

    // set weight (from 100)

    for (auto &it : *this)
        it.second = 100 * it.second / max;

    // get max
    max = 0;

    for (auto &it : *this)
        max += it.second;

    const u8 rand = Rand::Get(max);
    u8 amount = 0;


    for (auto &it : *this)
    {
        amount += it.second;
        if (rand <= amount) return it.first;
    }

    ERROR("weight not found, return 0");
    return 0;
}
