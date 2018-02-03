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
#include "bitmodes.h"
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

    void PushBack(const Monster &, u32);

    void PopBack();

    size_t Size() const;

    Troop *GetTroop(size_t);

    const Troop *GetTroop(size_t) const;

    void UpgradeMonsters(const Monster &);

    u32 GetCountMonsters(const Monster &) const;

    u32 GetCount() const;

    bool isValid() const;

    bool HasMonster(const Monster &) const;

    bool AllTroopsIsRace(int) const;

    u32 GetUniqueCount() const;

    bool JoinTroop(const Troop &);

    bool JoinTroop(const Monster &, u32);

    bool CanJoinTroop(const Monster &) const;

    void JoinTroops(Troops &);

    bool CanJoinTroops(const Troops &) const;

    Troops GetOptimized() const;

    virtual u32 GetAttack() const;

    virtual u32 GetDefense() const;

    u32 GetHitPoints() const;

    u32 GetDamageMin() const;

    u32 GetDamageMax() const;

    u32 GetStrength() const;

    void Clean();

    void UpgradeTroops(const Castle &);

    Troop *GetFirstValid();

    Troop *GetWeakestTroop();

    Troop *GetSlowestTroop();

    void ArrangeForBattle(bool = false);

    void JoinStrongest(Troops &, bool);

    void KeepOnlyWeakest(Troops &, bool);

    void DrawMons32LineWithScoute(s32, s32, u32, u32, u32, u32, bool) const;

    void SplitTroopIntoFreeSlots(const Troop &, u32);

    vector<sp<Troop>> _items;
};

enum
{
    JOIN_NONE, JOIN_FREE, JOIN_COST, JOIN_FLEE
};

struct JoinCount : pair<int, u32>
{
    JoinCount() : pair<int, u32>(JOIN_NONE, 0)
    {}

    JoinCount(int reason, u32 count) : pair<int, u32>(reason, count)
    {}
};

class Army : public Troops, public Control
{
public:
    static string SizeString(u32);

    static string TroopSizeString(const Troop &);

    // compare
    static bool WeakestTroop(const Troop *, const Troop *);

    static bool StrongestTroop(const Troop *, const Troop *);

    static bool SlowestTroop(const Troop *, const Troop *);

    static bool FastestTroop(const Troop *, const Troop *);

    static void SwapTroops(Troop &, Troop &);

    static u32 UniqueCount(const Army &);

    // 0: fight, 1: free join, 2: join with gold, 3: flee
    static JoinCount GetJoinSolution(const Heroes &, const Maps::Tiles &, const Troop &);

    static bool TroopsStrongerEnemyTroops(const Troops &, const Troops &);

    static void DrawMons32Line(const Troops &, s32, s32, u32, u32 = 0, u32 = 0);

    static void DrawMons32LineWithScoute(const Troops &, s32, s32, u32, u32, u32, u32);

    static void DrawMons32LineShort(const Troops &, s32, s32, u32, u32, u32);

    Army(HeroBase *s = nullptr);

    Army(const Maps::Tiles &);

    ~Army();

    void Reset(bool = false);    // reset: soft or hard

    int GetRace() const;

    int GetColor() const;

    int GetControl() const;

    u32 GetAttack() const;

    u32 GetDefense() const;

    void SetColor(int);

    int GetMorale() const;

    int GetLuck() const;

    int GetMoraleModificator(string *) const;

    static int GetLuckModificator(string *);

    u32 ActionToSirens();

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
