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

#ifndef H2ARMY_H
#define H2ARMY_H

#include <utility>
#include <string>
#include <vector>
#include "players.h"
#include "army_troop.h"

class Castle;

class HeroBase;

class Heroes;

namespace Maps
{
    class Tiles;
}

class Troops
{
public:
    Troops();

    virtual ~Troops();

    void Assign(const Troop *, const Troop *);

    void Assign(const Troops &);

    void Insert(const Troops &);

    void PushBack(const Monster &, uint32_t);

    void PopBack();

    size_t Size() const;

    Troop *GetTroop(size_t);

    const Troop *GetTroop(size_t) const;

    void UpgradeMonsters(const Monster &);

    uint32_t GetCountMonsters(const Monster &) const;

    uint32_t GetCount() const;

    bool isValid() const;

    bool HasMonster(const Monster &) const;

    bool AllTroopsIsRace(int) const;

    uint32_t GetUniqueCount() const;

    bool JoinTroop(const Troop &);

    bool JoinTroop(const Monster &, uint32_t);

    bool CanJoinTroop(const Monster &) const;

    void JoinTroops(Troops &);

    bool CanJoinTroops(const Troops &) const;

    Troops GetOptimized() const;

    virtual uint32_t GetAttack() const;

    virtual uint32_t GetDefense() const;

    uint32_t GetHitPoints() const;

    uint32_t GetDamageMin() const;

    uint32_t GetDamageMax() const;

    uint32_t GetStrength() const;

    void Clean();

    void UpgradeTroops(const Castle &);

    Troop *GetFirstValid();

    Troop *GetWeakestTroop();

    Troop *GetSlowestTroop();

    void ArrangeForBattle(bool = false);

    void JoinStrongest(Troops &, bool);

    void KeepOnlyWeakest(Troops &, bool);

    void DrawMons32LineWithScoute(s32, s32, uint32_t, uint32_t, uint32_t, uint32_t, bool) const;

    void SplitTroopIntoFreeSlots(const Troop &, uint32_t);

    vector<sp<Troop>> _items;
};

enum
{
    JOIN_NONE, JOIN_FREE, JOIN_COST, JOIN_FLEE
};

struct JoinCount : pair<int, uint32_t>
{
    JoinCount() : pair<int, uint32_t>(JOIN_NONE, 0)
    {}

    JoinCount(int reason, uint32_t count) : pair<int, uint32_t>(reason, count)
    {}
};

class Army : public Control
{
public:
    Troops m_troops;

    static string SizeString(uint32_t);

    static string TroopSizeString(const Troop &);

    // compare
    static bool WeakestTroop(const Troop *, const Troop *);

    static bool StrongestTroop(const Troop *, const Troop *);

    static bool SlowestTroop(const Troop *, const Troop *);

    static bool FastestTroop(const Troop *, const Troop *);

    static void SwapTroops(Troop &, Troop &);

    static uint32_t UniqueCount(const Army &);

    // 0: fight, 1: free join, 2: join with gold, 3: flee
    static JoinCount GetJoinSolution(const Heroes &, const Maps::Tiles &, const Troop &);

    static bool TroopsStrongerEnemyTroops(const Troops &, const Troops &);

    static void DrawMons32Line(const Troops &, s32, s32, uint32_t, uint32_t = 0, uint32_t = 0);

    static void DrawMons32LineWithScoute(const Troops &, s32, s32, uint32_t, uint32_t, uint32_t, uint32_t);

    static void DrawMons32LineShort(const Troops &, s32, s32, uint32_t, uint32_t, uint32_t);

    Army(HeroBase *s = nullptr);

    Army(const Maps::Tiles &);

    ~Army();

    void Reset(bool = false);    // reset: soft or hard

    int GetRace() const;

    int GetColor() const;

    int GetControl() const;

    uint32_t GetAttack() const;

    uint32_t GetDefense() const;

    void SetColor(int);

    int GetMorale() const;

    int GetLuck() const;

    int GetMoraleModificator(string *) const;

    static int GetLuckModificator(string *);

    uint32_t ActionToSirens();

    const HeroBase *GetCommander() const;

    HeroBase *GetCommander();

    void SetCommander(HeroBase *);

    const Castle *inCastle() const;

    string String() const;

    void JoinStrongestFromArmy(Army &);

    void KeepOnlyWeakestTroops(Army &);

    void SetSpreadFormat(bool);

    bool isSpreadFormat() const;

    bool isFullHouse() const;

    bool SaveLastTroop() const;

protected:
    friend StreamBase &operator<<(StreamBase &, const Army &);

    friend ByteVectorReader &operator>>(ByteVectorReader &msg, Army &army);

    HeroBase *commander;
    bool combat_format;
    int color;

private:
    Army &operator=(const Army &)
    { return *this; }
};

StreamBase &operator<<(StreamBase &, const Army &);

ByteVectorReader &operator>>(ByteVectorReader &, Army &);

#endif
