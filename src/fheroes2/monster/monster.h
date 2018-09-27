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

#include "ByteVectorReader.h"
#include <string>
#include "payment.h"

class Spell;

class Monster
{
public:
    enum
    {
        JOIN_CONDITION_SKIP = 0,
        JOIN_CONDITION_MONEY = 1,
        JOIN_CONDITION_FREE = 2,
        JOIN_CONDITION_FORCE = 3
    };

    enum level_t
    {
        LEVEL0,
        LEVEL1,
        LEVEL2,
        LEVEL3,
        LEVEL4
    };

    enum monster_t
    {
        UNKNOWN,

        PEASANT,
        ARCHER,
        RANGER,
        PIKEMAN,
        VETERAN_PIKEMAN,
        SWORDSMAN,
        MASTER_SWORDSMAN,
        CAVALRY,
        CHAMPION,
        PALADIN,
        CRUSADER,
        GOBLIN,
        ORC,
        ORC_CHIEF,
        WOLF,
        OGRE,
        OGRE_LORD,
        TROLL,
        WAR_TROLL,
        CYCLOPS,
        SPRITE,
        DWARF,
        BATTLE_DWARF,
        ELF,
        GRAND_ELF,
        DRUID,
        GREATER_DRUID,
        UNICORN,
        PHOENIX,
        CENTAUR,
        GARGOYLE,
        GRIFFIN,
        MINOTAUR,
        MINOTAUR_KING,
        HYDRA,
        GREEN_DRAGON,
        RED_DRAGON,
        BLACK_DRAGON,
        HALFLING,
        BOAR,
        IRON_GOLEM,
        STEEL_GOLEM,
        ROC,
        MAGE,
        ARCHMAGE,
        GIANT,
        TITAN,
        SKELETON,
        ZOMBIE,
        MUTANT_ZOMBIE,
        MUMMY,
        ROYAL_MUMMY,
        VAMPIRE,
        VAMPIRE_LORD,
        LICH,
        POWER_LICH,
        BONE_DRAGON,

        ROGUE,
        NOMAD,
        GHOST,
        GENIE,
        MEDUSA,
        EARTH_ELEMENT,
        AIR_ELEMENT,
        FIRE_ELEMENT,
        WATER_ELEMENT,

        MONSTER_RND1,
        MONSTER_RND2,
        MONSTER_RND3,
        MONSTER_RND4,
        MONSTER_RND
    };

    Monster(int = UNKNOWN);

    explicit Monster(const Spell&);

    Monster(int race, uint32_t dw);

    virtual ~Monster() = default;

    bool operator<(const Monster&) const;

    bool operator==(const Monster&) const;

    bool operator!=(const Monster&) const;

    int operator()() const;

    int GetID() const;

    void Upgrade();

    Monster GetUpgrade() const;

    Monster GetDowngrade() const;

    uint32_t GetAttack() const;

    uint32_t GetDefense() const;

    static int GetColor();

    static int GetMorale();

    static int GetLuck();

    int GetRace() const;

    uint32_t GetDamageMin() const;

    uint32_t GetDamageMax() const;

    uint32_t GetShots() const;

    static uint32_t GetHitPoints(const Monster& m);

    uint32_t GetHitPoints() const;

    uint32_t GetSpeed() const;

    uint32_t GetGrown() const;

    int GetLevel() const;

    uint32_t GetRNDSize(bool skip) const;

    std::string GetName() const;

    std::string GetMultiName() const;

    std::string GetPluralName(uint32_t) const;

    bool isValid() const;

    bool isElemental() const;

    bool isUndead() const;

    bool isFly() const;

    bool isWide() const;

    bool isArchers() const;

    bool isAllowUpgrade() const;

    bool isTwiceAttack() const;

    bool isResurectLife() const;

    bool isDoubleCellAttack() const;

    bool isMultiCellAttack() const;

    bool isAlwayResponse() const;

    bool isHideAttack() const;

    bool isDragons() const;

    bool isAffectedByMorale() const;

    bool isAlive() const;

    int ICNMonh() const;

    uint32_t GetSpriteIndex() const;

    payment_t GetCost() const;

    payment_t GetUpgradeCost() const;

    uint32_t GetDwelling() const;

    static Monster Rand(level_t = LEVEL0);

    static uint32_t Rand4WeekOf();

    static uint32_t Rand4MonthOf();

    static uint32_t GetCountFromHitPoints(const Monster&, uint32_t);

    static void UpdateStats(const string&);

    static float GetUpgradeRatio();

    int id = 0;
protected:
    static Monster FromDwelling(int race, uint32_t dw);
};

struct MonsterStaticData
{
    // wrapper for stream
    static MonsterStaticData& Get();
};

ByteVectorWriter& operator<<(ByteVectorWriter&, const Monster&);
ByteVectorReader& operator>>(ByteVectorReader&, Monster&);

ByteVectorWriter& operator<<(ByteVectorWriter&, const MonsterStaticData&);
ByteVectorReader& operator>>(ByteVectorReader&, MonsterStaticData&);
