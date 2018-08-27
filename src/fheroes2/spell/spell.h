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
#include "ByteVectorReader.h"
#include "ByteVectorWriter.h"

class HeroBase;

class StreamBase;

class StreamBase;

class Spell
{
public:
    enum type_t
    {
        NONE = 0,
        FIREBALL,
        FIREBLAST,
        LIGHTNINGBOLT,
        CHAINLIGHTNING,
        TELEPORT,
        CURE,
        MASSCURE,
        RESURRECT,
        RESURRECTTRUE,
        HASTE,
        MASSHASTE,
        SLOW,
        MASSSLOW,
        BLIND,
        BLESS,
        MASSBLESS,
        STONESKIN,
        STEELSKIN,
        CURSE,
        MASSCURSE,
        HOLYWORD,
        HOLYSHOUT,
        ANTIMAGIC,
        DISPEL,
        MASSDISPEL,
        ARROW,
        BERSERKER,
        ARMAGEDDON,
        ELEMENTALSTORM,
        METEORSHOWER,
        PARALYZE,
        HYPNOTIZE,
        COLDRAY,
        COLDRING,
        DISRUPTINGRAY,
        DEATHRIPPLE,
        DEATHWAVE,
        DRAGONSLAYER,
        BLOODLUST,
        ANIMATEDEAD,
        MIRRORIMAGE,
        SHIELD,
        MASSSHIELD,
        SUMMONEELEMENT,
        SUMMONAELEMENT,
        SUMMONFELEMENT,
        SUMMONWELEMENT,
        EARTHQUAKE,
        VIEWMINES,
        VIEWRESOURCES,
        VIEWARTIFACTS,
        VIEWTOWNS,
        VIEWHEROES,
        VIEWALL,
        IDENTIFYHERO,
        SUMMONBOAT,
        DIMENSIONDOOR,
        TOWNGATE,
        TOWNPORTAL,
        VISIONS,
        HAUNT,
        SETEGUARDIAN,
        SETAGUARDIAN,
        SETFGUARDIAN,
        SETWGUARDIAN,

        RANDOM,
        RANDOM1,
        RANDOM2,
        RANDOM3,
        RANDOM4,
        RANDOM5,

        STONE
    };

    Spell(int = NONE);

    bool operator<(const Spell&) const;

    bool operator==(const Spell&) const;

    bool operator!=(const Spell&) const;

    int operator()() const;

    int GetID() const;

    std::string GetName() const;

    std::string GetDescription() const;

    uint32_t SpellPoint(const HeroBase* hero = nullptr) const;

    uint32_t MovePoint() const;

    int Level() const;

    uint32_t Damage() const;

    uint32_t Restore() const;

    uint32_t Resurrect() const;

    uint32_t ExtraValue() const;

    payment_t GetCost() const;

    bool isValid() const;

    bool isLevel(int) const;

    bool isCombat() const;

    bool isAdventure() const;

    bool isDamage() const;

    bool isRestore() const;

    bool isResurrect() const;

    bool isMindInfluence() const;

    bool isUndeadOnly() const;

    bool isALiveOnly() const;

    bool isSummon() const;

    bool isApplyWithoutFocusObject() const;

    bool isApplyToAnyTroops() const;

    bool isApplyToFriends() const;

    bool isApplyToEnemies() const;

    bool isMassActions() const;

    bool isRaceCompatible(int race) const;

    /* return index sprite spells.icn */
    uint32_t IndexSprite() const;

    /* return index in spellinl.icn */
    uint32_t InlIndexSprite() const;

    static Spell RandCombat(int lvl);

    static Spell RandAdventure(int lvl);

    static Spell Rand(int lvl, bool adv);

    static void UpdateStats(const string&);

    static uint32_t CalculateDimensionDoorDistance(uint32_t current_sp, uint32_t total_hp);

    void ReadFrom(ByteVectorReader& msg);

private:
    friend ByteVectorWriter& operator<<(ByteVectorWriter&, const Spell&);


    friend ByteVectorReader& operator>>(ByteVectorReader&, Spell&);

    int id;
};

ByteVectorWriter& operator<<(ByteVectorWriter&, const Spell&);
ByteVectorReader& operator>>(ByteVectorReader&, Spell&);
