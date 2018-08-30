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

#include "direction.h"
#include "skill.h"
#include "artifact.h"
#include "army_troop.h"
#include "resource.h"
#include "ByteVectorReader.h"
#include "ByteVectorWriter.h"

class Sprite;

class Heroes;

class Spell;

class Monster;

namespace MP2
{
    struct mp2tile_t;
    struct mp2addon_t;
}

namespace Maps
{
    struct TilesAddon
    {
        enum level_t
        {
            GROUND = 0,
            DOWN = 1,
            SHADOW = 2,
            UPPER = 3
        };

        TilesAddon();

        TilesAddon(int lv, uint32_t gid, int obj, uint32_t ii);

        TilesAddon& operator=(const TilesAddon& ta);

        bool isUniq(uint32_t) const;

        bool isRoad(int) const;

        bool isICN(int) const;

        string String(int level) const;

        static bool isStream(const TilesAddon&);

        static bool isRoad(const TilesAddon&);

        static bool isResource(const TilesAddon&);

        static bool isWaterResource(const TilesAddon&);

        static bool isWhirlPool(const TilesAddon&);

        static bool isStandingStone(const TilesAddon&);

        static bool isArtifact(const TilesAddon&);

        static bool isCampFire(const TilesAddon&);

        static bool isMonster(const TilesAddon&);

        static bool isArtesianSpring(const TilesAddon&);

        static bool isOasis(const TilesAddon&);

        static bool isWateringHole(const TilesAddon&);

        static bool isJail(const TilesAddon&);

        static bool isMine(const TilesAddon&);

        static bool isShadow(const TilesAddon&);

        static bool isEvent(const TilesAddon&);

        static bool isBoat(const TilesAddon&);

        static bool isMiniHero(const TilesAddon&);

        static bool isRandomResource(const TilesAddon&);

        static bool isRandomArtifact(const TilesAddon&);

        static bool isRandomArtifact1(const TilesAddon&);

        static bool isRandomArtifact2(const TilesAddon&);

        static bool isRandomArtifact3(const TilesAddon&);

        static bool isUltimateArtifact(const TilesAddon&);

        static bool isCastle(const TilesAddon&);

        static bool isRandomCastle(const TilesAddon&);

        static bool isRandomMonster(const TilesAddon&);

        static bool isSkeleton(const TilesAddon&);

        static bool isSkeletonFix(const TilesAddon&);

        static bool isFlag32(const TilesAddon&);

        static bool isX_LOC123(const TilesAddon&);

        static bool isAbandoneMineSprite(const TilesAddon&);

        static bool isMounts(const TilesAddon&);

        static bool isRocs(const TilesAddon&);

        static bool isForests(const TilesAddon&);

        static bool isTrees(const TilesAddon&);

        static bool isDeadTrees(const TilesAddon&);

        static bool isCactus(const TilesAddon&);

        static bool isStump(const TilesAddon&);

        static int GetPassable(const TilesAddon&);

        static int GetActionObject(const TilesAddon&);

        static int GetLoyaltyObject(const TilesAddon&);

        static bool isBarrier(const TilesAddon&);

        static int ColorFromBarrierSprite(const TilesAddon&);

        static int ColorFromTravellerTentSprite(const TilesAddon&);

        static pair<int, int>
        ColorRaceFromHeroSprite(const TilesAddon&);

        static bool PredicateSortRules1(const TilesAddon&, const TilesAddon&);

        static bool PredicateSortRules2(const TilesAddon&, const TilesAddon&);

        static void UpdateFountainSprite(TilesAddon&);

        static void UpdateTreasureChestSprite(TilesAddon&);

        static int UpdateStoneLightsSprite(TilesAddon&);

        static void UpdateAbandoneMineLeftSprite(TilesAddon&, int resource);

        static void UpdateAbandoneMineRightSprite(TilesAddon&);

        static bool ForceLevel1(const TilesAddon&);

        static bool ForceLevel2(const TilesAddon&);

        uint32_t uniq;
        u8 level;
        u8 object;
        u8 index;
        u8 tmp;
    };

    struct Addons 
    {
		vector<TilesAddon> _items;
        void Remove(uint32_t uniq);
    };

    class Tiles
    {
    public:
        Tiles();

        void Init(s32, const MP2::mp2tile_t&);

        s32 GetIndex() const;

        Point GetCenter() const;

        int GetObject(bool skip_hero = true) const;

        uint32_t GetObjectUID(int obj) const;

        int GetQuantity1() const
        {
            return quantity1;
        }

        int GetQuantity2() const
        {
            return quantity2;
        }

        int GetPassable() const;

        int GetGround() const;

        bool isWater() const;

        uint32_t TileSpriteIndex() const;

        uint32_t TileSpriteShape() const;

        Surface GetTileSurface() const;

        bool isPassable(const Heroes&) const;

        bool isPassable(const Heroes*, int direct, bool skipfog) const;

        bool isRoad(int = DIRECTION_ALL) const;

        bool isObject(int obj) const
        {
            return obj == mp2_object;
        }

        bool isStream() const;

        bool GoodForUltimateArtifact() const;

        TilesAddon* FindAddonICN1(int icn1);

        TilesAddon* FindAddonICN2(int icn2);

        TilesAddon* FindAddonLevel1(uint32_t uniq1);

        TilesAddon* FindAddonLevel2(uint32_t uniq2);

        TilesAddon* FindObject(int) const;

        const TilesAddon* FindObjectConst(int) const;

        void SetTile(uint32_t sprite_index, uint32_t shape /* 0: none, 1 : vert, 2: horz, 3: both */);

        void SetObject(int object);

        void SetIndex(int);

        void FixObject();

        void UpdatePassable();

        void CaptureFlags32(int obj, int col);

        void RedrawTile(Surface&) const;

        void RedrawBottom(Surface&, bool skip_objs = false) const;

        void RedrawBottom4Hero(Surface&) const;

        void RedrawTop(Surface&, const TilesAddon* skip = nullptr) const;

        void RedrawTop4Hero(Surface&, bool skip_ground) const;

        void RedrawObjects(Surface&) const;

        void RedrawFogs(Surface&, int) const;

        static void RedrawPassable(Surface&);

        void AddonsPushLevel1(const MP2::mp2tile_t&);

        void AddonsPushLevel1(const MP2::mp2addon_t&);

        void AddonsPushLevel1(const TilesAddon&);

        void AddonsPushLevel2(const MP2::mp2tile_t&);

        void AddonsPushLevel2(const MP2::mp2addon_t&);

        void AddonsPushLevel2(const TilesAddon&);

        void AddonsSort();

        void Remove(uint32_t uniq);

        void RemoveObjectSprite();

        string String() const;

        bool isFog(int color) const;

        void ClearFog(int color);

        /* monster operation */
        bool MonsterJoinConditionSkip() const;

        bool MonsterJoinConditionMoney() const;

        bool MonsterJoinConditionFree() const;

        bool MonsterJoinConditionForce() const;

        int MonsterJoinCondition() const;

        void MonsterSetJoinCondition(int) const;

        void MonsterSetFixedCount() const;

        bool MonsterFixedCount() const;

        void MonsterSetCount(uint32_t count);

        uint32_t MonsterCount() const;

        bool CaptureObjectIsProtection() const;

        /* object quantity operation */
        void QuantityUpdate();

        void QuantityReset();

        bool QuantityIsValid() const;

        void QuantitySetColor(int);

        int QuantityTeleportType() const;

        int QuantityVariant() const;

        int QuantityExt() const;

        int QuantityColor() const;

        uint32_t QuantityGold() const;

        Spell QuantitySpell() const;

        Skill::Secondary QuantitySkill() const;

        Artifact QuantityArtifact() const;

        ResourceCount QuantityResourceCount() const;

        Funds QuantityFunds() const;

        Monster QuantityMonster() const;

        Troop QuantityTroop() const;

        void SetObjectPassable(bool);

        Heroes* GetHeroes() const;

        void SetHeroes(Heroes*);

        static void PlaceMonsterOnTile(Tiles&, const Monster&, uint32_t);

        static void UpdateAbandoneMineSprite(Tiles&);

        static void FixedPreload(Tiles&);

    private:
        TilesAddon* FindFlags();

        void CorrectFlags32(uint32_t index, bool);

        void RemoveJailSprite();

        void RemoveBarrierSprite();

        bool isLongObject(int direction);

        void RedrawBoat(Surface&) const;

        void RedrawMonster(Surface&) const;

        void QuantitySetVariant(int);

        void QuantitySetExt(int);

        void QuantitySetSkill(int);

        void QuantitySetSpell(int);

        void QuantitySetArtifact(int);

        void QuantitySetResource(int, uint32_t);

        void QuantitySetTeleportType(int);

        int GetQuantity3() const;

        void SetQuantity3(int);

        static void UpdateMonsterInfo(Tiles&);

        static void UpdateDwellingPopulation(Tiles&);

        static void UpdateMonsterPopulation(Tiles&);

        static void UpdateRNDArtifactSprite(Tiles&);

        static void UpdateRNDResourceSprite(Tiles&);

        static void UpdateStoneLightsSprite(Tiles&);

        static void UpdateFountainSprite(Tiles&);

        static void UpdateTreasureChestSprite(Tiles&);

        friend ByteVectorWriter& operator<<(ByteVectorWriter&, const Tiles&);

        friend ByteVectorReader& operator>>(ByteVectorReader&, Tiles&);

        Addons addons_level1;
        Addons addons_level2; // 16

        uint32_t maps_index = 0;
        u16 pack_sprite_index = 0;

        u16 tile_passable = 0;
        u8 mp2_object = 0;
        u8 fog_colors = 0;

        u8 quantity1 = 0;
        u8 quantity2 = 0;
        u8 quantity3 = 0;
    };

    ByteVectorWriter& operator<<(ByteVectorWriter&, const TilesAddon&);
    ByteVectorWriter& operator<<(ByteVectorWriter&, const Tiles&);

    ByteVectorReader& operator>>(ByteVectorReader&, TilesAddon&);
    ByteVectorReader& operator>>(ByteVectorReader&, Tiles&);
}
