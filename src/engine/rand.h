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

#include <vector>
#include <utility>
#include <iterator>
#include "types.h"

using namespace std;

namespace Rand
{
    void Init();

    int32_t Get(int32_t min, int32_t max = 0);

    template <typename T>
    const T* Get(const vector<T>& vec)
    {
        typename vector<T>::const_iterator it = vec.begin();
        std::advance(it, Rand::Get(vec.size() - 1));
        return it == vec.end() ? nullptr : &(*it);
    }

    typedef pair<s32, uint32_t> ValuePercent;

    class Queue : private vector<ValuePercent>
    {
    public:
        explicit Queue(uint32_t size = 0);

        void Reset();

        void Push(s32, uint32_t);

        size_t Size() const;

        s32 Get();
    };
}
