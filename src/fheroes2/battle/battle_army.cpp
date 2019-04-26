/***************************************************************************
 *   Copyright (C) 2012 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#include <algorithm>
#include "speed.h"
#include "settings.h"
#include "heroes.h"
#include "battle_troop.h"
#include "rand.h"
#include "battle_army.h"
#include <functional>

#define CAPACITY 16

Battle::Units::Units()
{
    _items.reserve(CAPACITY);
}

Battle::Units::Units(const Units& units, bool filter)
{
    _items.reserve(CAPACITY < units._items.size() ? units._items.size() : CAPACITY);
    _items.assign(units._items.begin(), units._items.end());
    if (filter)
        _items.resize(distance(_items.begin(),
                               remove_if(_items.begin(), _items.end(),
                                         [](const Unit* unit) { return !unit->isValid(); })));
}

Battle::Units::Units(const Units& units1, const Units& units2)
{
    const size_t capacity = units1._items.size() + units2._items.size();
    _items.reserve(CAPACITY < capacity ? capacity : CAPACITY);
    _items.insert(_items.end(), units1._items.begin(), units1._items.end());
    _items.insert(_items.end(), units2._items.begin(), units2._items.end());
}

Battle::Units& Battle::Units::operator=(const Units& units)
{
    _items.reserve(CAPACITY < units._items.size() ? units._items.size() : CAPACITY);
    _items.assign(units._items.begin(), units._items.end());

    return *this;
}

struct FastestUnits
{
    bool f;

    FastestUnits(bool v) : f(v)
    {
    }

    bool operator()(const Battle::Unit* t1, const Battle::Unit* t2) const
    {
        return t1->GetSpeed(f) > t2->GetSpeed(f);
    }
};

struct SlowestUnits
{
    bool f;

    SlowestUnits(bool v) : f(v)
    {
    }

    bool operator()(const Battle::Unit* t1, const Battle::Unit* t2) const
    {
        return t1->GetSpeed(f) < t2->GetSpeed(f);
    }
};

void Battle::Units::SortSlowest(bool f)
{
    SlowestUnits CompareFunc(f);

    sort(_items.begin(), _items.end(), CompareFunc);
}

void Battle::Units::SortFastest(bool f)
{
    FastestUnits CompareFunc(f);

    sort(_items.begin(), _items.end(), CompareFunc);
}

void Battle::Units::SortStrongest()
{
    sort(_items.begin(), _items.end(), Army::StrongestTroop);
}

void Battle::Units::SortWeakest()
{
    sort(_items.begin(), _items.end(), Army::WeakestTroop);
}

Battle::Unit* Battle::Units::FindUID(uint32_t pid)
{
    auto it = find_if(_items.begin(), _items.end(),
                      [&](const Unit* unit)
                      {
                          return unit->isUID(pid);
                      });

    return it == _items.end() ? nullptr : *it;
}

Battle::Unit* Battle::Units::FindMode(uint32_t mod)
{
    auto it = find_if(_items.begin(), _items.end(),
                      [&](const Unit* unit)
                      {
                          return unit->Modes(mod);
                      });
    return it == _items.end() ? nullptr : *it;
}


