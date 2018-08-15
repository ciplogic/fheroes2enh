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
#include <string>
#include "mageguild.h"
#include "captain.h"
#include "army.h"
#include "sprite.h"
#include "castle_heroes.h"
#include "position.h"
#include "ByteVectorReader.h"
#include "heroes.h"

namespace Maps
{
    class Tiles;
}

class MeetingButton : public ButtonSprite
{
public:
    MeetingButton(s32, s32);
};

class SwapButton : public ButtonSprite
{
public:
    SwapButton(s32, s32);
};

enum building_t
{
    BUILD_NOTHING = 0x00000000,
    BUILD_THIEVESGUILD = 0x00000001,
    BUILD_TAVERN = 0x00000002,
    BUILD_SHIPYARD = 0x00000004,
    BUILD_WELL = 0x00000008,
    BUILD_STATUE = 0x00000010,
    BUILD_LEFTTURRET = 0x00000020,
    BUILD_RIGHTTURRET = 0x00000040,
    BUILD_MARKETPLACE = 0x00000080,
    BUILD_WEL2 = 0x00000100,   // Farm, Garbage He, Crystal Gar, Waterfall, Orchard, Skull Pile
    BUILD_MOAT = 0x00000200,
    BUILD_SPEC = 0x00000400,   // Fortification, Coliseum, Rainbow, Dungeon, Library, Storm
    BUILD_CASTLE = 0x00000800,
    BUILD_CAPTAIN = 0x00001000,
    BUILD_SHRINE = 0x00002000,
    BUILD_MAGEGUILD1 = 0x00004000,
    BUILD_MAGEGUILD2 = 0x00008000,
    BUILD_MAGEGUILD3 = 0x00010000,
    BUILD_MAGEGUILD4 = 0x00020000,
    BUILD_MAGEGUILD5 = 0x00040000,
    BUILD_MAGEGUILD = BUILD_MAGEGUILD1 | BUILD_MAGEGUILD2 | BUILD_MAGEGUILD3 | BUILD_MAGEGUILD4 | BUILD_MAGEGUILD5,
    BUILD_TENT = 0x00080000,    // deprecated
    DWELLING_MONSTER1 = 0x00100000,
    DWELLING_MONSTER2 = 0x00200000,
    DWELLING_MONSTER3 = 0x00400000,
    DWELLING_MONSTER4 = 0x00800000,
    DWELLING_MONSTER5 = 0x01000000,
    DWELLING_MONSTER6 = 0x02000000,
    DWELLING_MONSTERS =
    DWELLING_MONSTER1 | DWELLING_MONSTER2 | DWELLING_MONSTER3 | DWELLING_MONSTER4 | DWELLING_MONSTER5 |
    DWELLING_MONSTER6,
    DWELLING_UPGRADE2 = 0x04000000,
    DWELLING_UPGRADE3 = 0x08000000,
    DWELLING_UPGRADE4 = 0x10000000,
    DWELLING_UPGRADE5 = 0x20000000,
    DWELLING_UPGRADE6 = 0x40000000,
    DWELLING_UPGRADE7 = 0x80000000,        // black dragon
    DWELLING_UPGRADES =
    DWELLING_UPGRADE2 | DWELLING_UPGRADE3 | DWELLING_UPGRADE4 | DWELLING_UPGRADE5 | DWELLING_UPGRADE6 |
    DWELLING_UPGRADE7
};

enum buildcond_t
{
    NOT_TODAY = -1,
    ALREADY_BUILT = -2,
    NEED_CASTLE = -3,
    BUILD_DISABLE = -4,
    UNKNOWN_UPGRADE = -5,
    REQUIRES_BUILD = -6,
    LACK_RESOURCES = -7,
    UNKNOWN_COND = 0,
    ALLOW_BUILD = 1
};

class Castle : public MapPosition, public BitModes, public ColorBase, public Control
{
public:
    enum flags_t
    {
        ALLOWCASTLE = 0x0002,
        CUSTOMARMY = 0x0004,
        ALLOWBUILD = 0x0008,
        DISABLEHIRES = 0x0010,
        CAPITAL = 0x0020
    };

    Castle();

    Castle(s32, s32, int rs);

    virtual ~Castle() = default;

    void LoadFromMP2(ByteVectorReader &);

    Captain &GetCaptain();

    const Captain &GetCaptain() const;

    bool isCastle() const;

    bool isCapital() const;

    bool HaveNearlySea() const;

    bool PresentBoat() const;

    bool AllowBuyHero(const Heroes &, string * = nullptr) const;

    bool isPosition(const Point &) const;

    bool isNecromancyShrineBuild() const;

    uint32_t CountBuildings() const;

    Heroes *RecruitHero(Heroes *);

    CastleHeroes GetHeroes() const;

    int GetRace() const;

    const string &GetName() const;

    int GetControl() const;

    int GetLevelMageGuild() const;

    const MageGuild &GetMageGuild() const;

    bool HaveLibraryCapability() const;

    bool isLibraryBuild() const;

    void MageGuildEducateHero(HeroBase &) const;

    const Army &GetArmy() const;

    Army &GetArmy();

    const Army &GetActualArmy() const;

    Army &GetActualArmy();

    uint32_t GetDwellingLivedCount(uint32_t) const;

    uint32_t GetActualDwelling(uint32_t) const;

    bool RecruitMonsterFromDwelling(uint32_t dw, uint32_t count);

    bool RecruitMonster(const Troop &);

    void RecruitAllMonster();

    void ChangeColor(int);

    void ActionNewDay();

    void ActionNewWeek();

    void ActionNewMonth();

    void ActionPreBattle();

    void ActionAfterBattle(bool attacker_wins);

    void DrawImageCastle(const Point &, Surface& destSurface) const;

    int OpenDialog(bool readonly = false, bool fade = false);

    static void DrawCastleFundsAtPos(Display &display, const Funds &resource, Rect src_rt);

    static int GetAttackModificator(string *);

    static int GetDefenseModificator(string *);

    int GetPowerModificator(string *) const;

    static int GetKnowledgeModificator(string *);

    int GetMoraleModificator(string *) const;

    int GetLuckModificator(string *) const;

    bool AllowBuild() const;

    bool AllowBuyBuilding(uint32_t) const;

    bool isBuild(uint32_t bd) const;

    bool BuyBuilding(uint32_t);

    bool AllowBuyBoat() const;

    bool BuyBoat() const;

    uint32_t GetBuildingRequires(uint32_t) const;

    int CheckBuyBuilding(uint32_t) const;

    static int GetAllBuildingStatus(const Castle &);

    void Scoute() const;

    string GetStringBuilding(uint32_t) const;

    string GetDescriptionBuilding(uint32_t) const;

    static string GetStringBuilding(uint32_t, int race);

    static string GetDescriptionBuilding(uint32_t, int race);

    static int GetICNBuilding(uint32_t, int race);

    static int GetICNBoat(int race);

    uint32_t GetUpgradeBuilding(uint32_t) const;

    static bool PredicateIsCastle(const Castle *);

    static bool PredicateIsTown(const Castle *);

    static bool PredicateIsBuildMarketplace(const Castle *);

    static bool PredicateIsCapital(const Castle *);

    static uint32_t GetGrownWell();

    static uint32_t GetGrownWel2();

    static uint32_t GetGrownWeekOf(const Monster &);

    static uint32_t GetGrownMonthOf();

    string String() const;

    int DialogBuyHero(const Heroes *) const;

    int DialogBuyCaptain(bool fixed = true) const;

    int DialogBuyCastle(bool fixed = true) const;

    void SwapCastleHeroes(CastleHeroes &);

private:
    uint32_t *GetDwelling(uint32_t dw);

    void EducateHeroes();

    Rect RedrawResourcePanel(const Point &) const;

    uint32_t OpenTown();

    void OpenTavern() const;

    void OpenThievesGuild();

    void OpenWell();

    void OpenMageGuild() const;

    void WellRedrawInfoArea(const Point &cur_pt);

    void JoinRNDArmy();

    void PostLoad();

private:
    friend ByteVectorWriter &operator<<(ByteVectorWriter &, const Castle &);
    friend ByteVectorReader &operator>>(ByteVectorReader &, Castle &);


    int race;
    uint32_t building;
    Captain captain;

    string name;

    MageGuild mageguild;
    uint32_t dwelling[CASTLEMAXMONSTER]{};
    Army army;
};

namespace CastleDialog
{
    struct builds_t
    {
        builds_t(building_t b, const Rect &r) : id(b), coord(r)
        {};

        bool operator==(uint32_t b) const
        { return b == id; };

        building_t id;
        Rect coord;
        Sprite contour;
    };

    struct CacheBuildings : vector<builds_t>
    {
        CacheBuildings(const Castle &, const Point &);

        const Rect &GetRect(building_t) const;
    };

    void RedrawAllBuilding(const Castle &, const Point &, const CacheBuildings &, uint32_t flash = BUILD_NOTHING);

    void RedrawAnimationBuilding(const Castle &, const Point &, const CacheBuildings &, uint32_t build);

    void RedrawBuildingSpriteToArea(const Sprite &, s32, s32, const Rect &);
}

struct VecCastles : vector<Castle *>
{
    Castle *Get(const Point &) const;

    Castle *GetFirstCastle() const;

    void ChangeColors(int, int);
};

struct AllCastles : VecCastles
{
    AllCastles();

    virtual ~AllCastles();

    void Init();

    void clear();

    void Scoute(int) const;
};

ByteVectorWriter &operator<<(ByteVectorWriter &, const VecCastles &);
ByteVectorReader &operator>>(ByteVectorReader &, VecCastles &);


ByteVectorWriter &operator<<(ByteVectorWriter &, const AllCastles &);
ByteVectorReader &operator>>(ByteVectorReader &, AllCastles &);

ByteVectorWriter &operator<<(ByteVectorWriter &, const Castle &);
ByteVectorReader &operator>>(ByteVectorReader &, Castle &);
