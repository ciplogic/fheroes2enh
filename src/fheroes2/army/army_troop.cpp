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

#include <sstream>

#include "speed.h"
#include "army.h"
#include "heroes_base.h"
#include "tools.h"

Troop::Troop() : Monster(UNKNOWN), count(0)
{
}

Troop::Troop(const Monster& m, uint32_t c) : Monster(m), count(c)
{
}

bool Troop::operator==(const Monster& m) const
{
    return static_cast<Monster>(*this) == m;
}

bool Troop::isMonster(int mons) const
{
    return GetID() == mons;
}

Monster Troop::operator()() const
{
    return *this;
}

Monster Troop::GetMonster() const
{
    return *this;
}

void Troop::Set(const Troop& t)
{
    SetMonster(t.GetMonster());
    SetCount(t.GetCount());
}

void Troop::Set(const Monster& m, uint32_t c)
{
    Set(Troop(m, c));
}

void Troop::SetMonster(const Monster& m)
{
    id = m();
}

void Troop::SetCount(uint32_t c)
{
    count = c;
}

void Troop::Reset()
{
    id = UNKNOWN;
    count = 0;
}

std::string Troop::GetName() const
{
    return GetPluralName(count);
}

uint32_t Troop::GetCount() const
{
    return count;
}

uint32_t Troop::GetHitPointsTroop() const
{
    return Monster::GetHitPoints(*this) * count;
}

uint32_t Troop::GetDamageMin() const
{
    return Monster::GetDamageMin() * count;
}

uint32_t Troop::GetDamageMax() const
{
    return Monster::GetDamageMax() * count;
}

uint32_t Troop::GetStrength() const
{
    double res = (GetDamageMin() + GetDamageMax()) >> 1;

    // increase strength
    if (isFly()) res += res * 0.5;
    if (isArchers()) res += res * 0.5;
    if (isTwiceAttack()) res += res * 0.5;
    if (isHideAttack()) res += res * 0.5;

    // slowly: decrease strength
    if (!isFly() && !isArchers() && Speed::AVERAGE > GetSpeed()) res -= res * 0.5;

    switch (GetID())
    {
    case GHOST:
        res *= 2;
        break;

    default:
        break;
    }

    return static_cast<uint32_t>(res);
}

bool Troop::isValid() const
{
    return Monster::isValid() && count;
}

payment_t Troop::GetCost() const
{
    return Monster::GetCost() * count;
}

payment_t Troop::GetUpgradeCost() const
{
    return Monster::GetUpgradeCost() * count;
}

bool Troop::isBattle() const
{
    return false;
}

bool Troop::isModes(uint32_t)
{
    return false;
}

string Troop::GetAttackString() const
{
    return Int2Str(GetAttack());
}

string Troop::GetDefenseString() const
{
    return Int2Str(GetDefense());
}

string Troop::GetShotString() const
{
    return Int2Str(GetShots());
}

string Troop::GetSpeedString() const
{
    return Speed::String(GetSpeed());
}

uint32_t Troop::GetHitPointsLeft()
{
    return 0;
}

uint32_t Troop::GetSpeed() const
{
    return Monster::GetSpeed();
}

uint32_t Troop::GetAffectedDuration(uint32_t)
{
    return 0;
}

/* ArmyTroop */
ArmyTroop::ArmyTroop(Army* a) : army(a)
{
}

ArmyTroop::ArmyTroop(Army* a, const Troop& t) : Troop(t), army(a)
{
}

ArmyTroop& ArmyTroop::operator=(const Troop& t)
{
    Set(t);
    return *this;
}

uint32_t ArmyTroop::GetAttack() const
{
    return Troop::GetAttack() + (army && army->GetCommander() ? army->GetCommander()->GetAttack() : 0);
}

uint32_t ArmyTroop::GetDefense() const
{
    return Troop::GetDefense() + (army && army->GetCommander() ? army->GetCommander()->GetDefense() : 0);
}

int ArmyTroop::GetColor() const
{
    return army ? army->GetColor() : Color::NONE;
}

int ArmyTroop::GetMorale() const
{
    return army && isAffectedByMorale() ? army->GetMorale() : Troop::GetMorale();
}

int ArmyTroop::GetLuck() const
{
    return army ? army->GetLuck() : Troop::GetLuck();
}

void ArmyTroop::SetArmy(const Army& a)
{
    army = &a;
}

const Army* ArmyTroop::GetArmy() const
{
    return army;
}

string ArmyTroop::GetAttackString() const
{
    if (Troop::GetAttack() == GetAttack())
        return Int2Str(Troop::GetAttack());

    ostringstream os;
    os << Troop::GetAttack() << " (" << GetAttack() << ")";
    return os.str();
}

string ArmyTroop::GetDefenseString() const
{
    if (Troop::GetDefense() == GetDefense())
        return Int2Str(Troop::GetDefense());

    ostringstream os;
    os << Troop::GetDefense() << " (" << GetDefense() << ")";
    return os.str();
}

ByteVectorWriter& operator<<(ByteVectorWriter& msg, const Troop& troop)
{
    return msg << troop.id << troop.count;
}

ByteVectorReader& operator>>(ByteVectorReader& msg, Troop& troop)
{
    return msg >> troop.id >> troop.count;
}
