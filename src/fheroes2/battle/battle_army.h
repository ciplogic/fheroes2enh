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

#pragma once

#include "army.h"
#include "bitmodes.h"

namespace Battle
{
    class Unit;

    class Units  
    {
    public:
        vector<Unit *> _items;

        Units();

        Units(const Units &, bool filter = false);

        Units(const Units &, const Units &);

        virtual ~Units() = default;

        Units &operator=(const Units &);

        Unit *FindMode(uint32_t);

        Unit *FindUID(uint32_t);

        void SortSlowest(bool);

        void SortFastest(bool);

        void SortStrongest();

        void SortWeakest();
    };

    enum
    {
        ARMY_GUARDIANS_OBJECT = 0x10000
    };

    class Force : public Units, public BitModes
    {
    public:
        Force(Army &, bool);

        ~Force();

        HeroBase *GetCommander();

        const HeroBase *GetCommander() const;

        bool isValid() const;

        bool HasMonster(const Monster &) const;

        uint32_t GetDeadHitPoints() const;

        uint32_t GetDeadCounts() const;

        int GetColor() const;

        int GetControl() const;

        uint32_t GetSurrenderCost() const;

        Troops GetKilledTroops() const;

        bool SetIdleAnimation();

        bool NextIdleAnimation();

        void NewTurn();

        void SyncArmyCount();

        static Unit *GetCurrentUnit(const Force &, const Force &, Unit *last, Units *all, bool part1);

        static Unit *GetCurrentUnit(const Force &, const Force &, Unit *last, bool part1);

        static void UpdateOrderUnits(const Force &, const Force &, Units &);

    private:
        Army &army;
        vector<uint32_t> uids;
    };

    ByteVectorWriter &operator<<(ByteVectorWriter &, const Force &);

    ByteVectorReader &operator>>(ByteVectorReader &, Force &);
}

