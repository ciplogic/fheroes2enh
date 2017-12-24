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

#ifndef H2ARMYTROOP_H
#define H2ARMYTROOP_H

#include <string>
#include "monster.h"
#include "ByteVectorReader.h"

class Army;

class Troop : public Monster
{
public:
    Troop();

    Troop(const Monster &, u32);

    bool operator==(const Monster &) const;

    Monster operator()() const;

    void Set(const Troop &);

    void Set(const Monster &, u32);

    void SetMonster(const Monster &);

    void SetCount(u32);

    void Reset();

    bool isMonster(int) const;

    const char *GetName() const;

    u32 GetCount() const;

    u32 GetHitPointsTroop() const;

    Monster GetMonster() const;

    u32 GetDamageMin() const;

    u32 GetDamageMax() const;

    u32 GetStrength() const;


    payment_t GetCost() const;

    payment_t GetUpgradeCost() const;

    bool isValid() const;

    virtual bool isBattle() const;

    bool isModes(u32) const;

    string GetAttackString() const;

    string GetDefenseString() const;

    string GetShotString() const;

    string GetSpeedString() const;

    u32 GetHitPointsLeft() const;

    u32 GetSpeed() const;

    u32 GetAffectedDuration(u32) const;

protected:
    friend StreamBase &operator<<(StreamBase &, const Troop &);

    friend StreamBase &operator>>(StreamBase &, Troop &);
	friend ByteVectorReader &operator>>(ByteVectorReader &msg, Troop &troop);

    u32 count;
};

StreamBase &operator<<(StreamBase &, const Troop &);

StreamBase &operator>>(StreamBase &, Troop &);
ByteVectorReader &operator>>(ByteVectorReader &msg, Troop &troop);

class ArmyTroop : public Troop
{
public:
    ArmyTroop(Army *);

    ArmyTroop(Army *, const Troop &);

    ArmyTroop &operator=(const Troop &);

    u32 GetAttack() const;

    u32 GetDefense() const;

    int GetColor() const;

    int GetMorale() const;

    int GetLuck() const;

    void SetArmy(const Army &);

    const Army *GetArmy() const;

    string GetAttackString() const;

    string GetDefenseString() const;

protected:
    const Army *army;
};

#endif
