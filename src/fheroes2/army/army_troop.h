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

#include <string>
#include "monster.h"
#include "ByteVectorReader.h"
#include "ByteVectorWriter.h"

class Army;

class Troop : public Monster
{
public:
    Troop();

    Troop(const Monster&, uint32_t);

    bool operator==(const Monster&) const;

    Monster operator()() const;

    void Set(const Troop&);

    void Set(const Monster&, uint32_t);

    void SetMonster(const Monster&);

    void SetCount(uint32_t);

    void Reset();

    bool isMonster(int) const;

    std::string GetName() const;

    uint32_t GetCount() const;

    uint32_t GetHitPointsTroop() const;

    Monster GetMonster() const;

    uint32_t GetDamageMin() const;

    uint32_t GetDamageMax() const;

    uint32_t GetStrength() const;


    payment_t GetCost() const;

    payment_t GetUpgradeCost() const;

    bool isValid() const;

    virtual bool isBattle() const;

    static bool isModes(uint32_t);

    string GetAttackString() const;

    string GetDefenseString() const;

    string GetShotString() const;

    string GetSpeedString() const;

    static uint32_t GetHitPointsLeft();

    uint32_t GetSpeed() const;

    static uint32_t GetAffectedDuration(uint32_t);

protected:
    friend ByteVectorWriter& operator<<(ByteVectorWriter&, const Troop&);

    friend ByteVectorReader& operator>>(ByteVectorReader& msg, Troop& troop);

    uint32_t count = 0;
};

ByteVectorWriter& operator<<(ByteVectorWriter&, const Troop&);

ByteVectorReader& operator>>(ByteVectorReader& msg, Troop& troop);

class ArmyTroop : public Troop
{
public:
    ArmyTroop(Army*);

    ArmyTroop(Army*, const Troop&);

    ArmyTroop& operator=(const Troop&);

    uint32_t GetAttack() const;

    uint32_t GetDefense() const;

    int GetColor() const;

    int GetMorale() const;

    int GetLuck() const;

    void SetArmy(const Army&);

    const Army* GetArmy() const;

    string GetAttackString() const;

    string GetDefenseString() const;

protected:
    const Army* army;
};
