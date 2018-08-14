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

#include "payment.h"
#include "puzzle.h"
#include "mp2.h"
#include "pairs.h"
#include "heroes.h"
#include "castle.h"
#include "heroes_recruits.h"
#include "ByteVectorReader.h"
#include "ByteVectorWriter.h"

class Player;

class Castle;

class Heroes;

struct AllHeroes;
struct VecHeroes;
struct AllCastles;
struct VecCastles;
struct CapturedObjects;

struct LastLoseHero : pair<int, uint32_t> /* Heroes, date */
{
    LastLoseHero() : pair<int, uint32_t>(Heroes::UNKNOWN, 0)
    {}
};

ByteVectorReader &operator>>(ByteVectorReader &, LastLoseHero &);

struct KingdomCastles : public VecCastles
{
    static KingdomCastles& GetNull();
};

struct KingdomHeroes : public VecHeroes
{
    static KingdomHeroes& GetNull();
};



class Kingdom : public BitModes, public Control
{
public:
    enum
    {
        //UNDEF	     = 0x0001,
                IDENTIFYHERO = 0x0002,
        DISABLEHIRES = 0x0004,
        OVERVIEWCSTL = 0x0008
    };

    Kingdom();

    virtual ~Kingdom()
    {}

    void Init(int color);

    void clear();

    void OverviewDialog();

    void UpdateStartingResource();

    bool isPlay() const;

    bool isLoss() const;

    bool AllowPayment(const Funds &) const;

    bool AllowRecruitHero(bool check_payment, int level) const;

    void SetLastLostHero(Heroes &);

    void ResetLastLostHero();

    void AddHeroStartCondLoss(Heroes *);

    string GetNamesHeroStartCondLoss() const;

    Heroes *GetLastLostHero() const;

    const Heroes *
    GetFirstHeroStartCondLoss() const;

    const Heroes *
    GetBestHero() const;

    int GetControl() const;

    int GetColor() const;

    int GetRace() const;

    const Funds &
    GetFunds() const
    { return resource; }

    Funds GetIncome(int = INCOME_ALL) const;

    uint32_t GetArmiesStrength() const;

    void AddFundsResource(const Funds &);

    void OddFundsResource(const Funds &);

    uint32_t GetCountCastle() const;

    uint32_t GetCountTown() const;

    uint32_t GetCountMarketplace() const;

    uint32_t GetCountCapital() const;

    uint32_t GetLostTownDays() const;

    uint32_t GetCountNecromancyShrineBuild() const;

    uint32_t GetCountBuilding(uint32_t) const;

    Recruits &GetRecruits();

    const KingdomHeroes &
    GetHeroes() const
    { return heroes; }

    const KingdomCastles &
    GetCastles() const
    { return castles; }

    KingdomHeroes &
    GetHeroes()
    { return heroes; }

    KingdomCastles &
    GetCastles()
    { return castles; }

    void AddHeroes(Heroes *);

    void RemoveHeroes(const Heroes *);

    void ApplyPlayWithStartingHero();

    void HeroesActionNewPosition() const;

    void AddCastle(const Castle *);

    void RemoveCastle(const Castle *);

    void ActionBeforeTurn();

    void ActionNewDay();

    void ActionNewWeek();

    void ActionNewMonth();

    void SetVisited(s32 index, int object = MP2::OBJ_ZERO);

    uint32_t CountVisitedObjects(int object) const;

    bool isVisited(int object) const;

    bool isVisited(const Maps::Tiles &) const;

    bool isVisited(s32, int obj) const;

    bool HeroesMayStillMove() const;

    const Puzzle &
    PuzzleMaps() const;

    Puzzle &PuzzleMaps();

    void SetVisitTravelersTent(int);

    bool IsVisitTravelersTent(int) const;

    void UpdateRecruits();

    void LossPostActions();

    static uint32_t GetMaxHeroes();

private:
    friend ByteVectorWriter &operator<<(ByteVectorWriter &, const Kingdom &);

    friend ByteVectorReader &operator>>(ByteVectorReader &, Kingdom &);

    int color;
    Funds resource;

    uint32_t lost_town_days;

    KingdomCastles castles;
    KingdomHeroes heroes;

    Recruits recruits;
    LastLoseHero lost_hero;

    list<IndexObject>
            visit_object;

    Puzzle puzzle_maps;
    uint32_t visited_tents_colors;

    KingdomHeroes heroes_cond_loss;
};

class Kingdoms
{
public:
    Kingdoms();

    void Init();

    void clear();

    void ApplyPlayWithStartingHero();

    void NewDay();

    void NewWeek();

    void NewMonth();

    Kingdom &GetKingdom(int color);

    const Kingdom &
    GetKingdom(int color) const;

    int GetLossColors() const;

    int GetNotLossColors() const;

    int FindWins(int) const;

    void AddHeroes(const AllHeroes &);

    void AddCastles(const AllCastles &);

    void AddCondLossHeroes(const AllHeroes &);

    void AddTributeEvents(CapturedObjects &, uint32_t day, int obj);

    uint32_t size() const;

private:
    friend ByteVectorWriter &operator<<(ByteVectorWriter &, const Kingdoms &);

    friend ByteVectorReader &operator>>(ByteVectorReader &, Kingdoms &);

    Kingdom kingdoms[KINGDOMMAX + 1];
};

ByteVectorWriter &operator<<(ByteVectorWriter &, const Kingdom &);
ByteVectorReader &operator>>(ByteVectorReader &, Kingdom &);

ByteVectorWriter &operator<<(ByteVectorWriter &, const Kingdoms &);
ByteVectorReader &operator>>(ByteVectorReader &, Kingdoms &);
