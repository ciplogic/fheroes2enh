/***************************************************************************
 *   Copyright (C) 2010 by Andrey Afletdinov <fheroes2@gmail.com>          *
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
#include <utility>
#include "bitmodes.h"
#include "sprite.h"
#include "battle.h"
#include "ByteVectorReader.h"
#include "ByteVectorWriter.h"
#include "battle_arena.h"
#include "players.h"

class Sprite;

class Spell;

class HeroBase;

namespace Battle
{
    struct ModeDuration : pair<uint32_t, uint32_t>
    {
        ModeDuration();

        ModeDuration(uint32_t, uint32_t);

        bool isMode(uint32_t) const;

        bool isZeroDuration() const;

        void DecreaseDuration();
    };

    struct ModesAffected : public vector<ModeDuration>
    {
        ModesAffected();

        uint32_t GetMode(uint32_t) const;

        void AddMode(uint32_t, uint32_t);

        void RemoveMode(uint32_t);

        void DecreaseDuration();

        uint32_t FindZeroDuration() const;
    };

    ByteVectorWriter& operator<<(ByteVectorWriter&, const ModesAffected&);
    ByteVectorReader& operator>>(ByteVectorReader&, ModesAffected&);

    enum
    {
        CONTOUR_MAIN = 0,
        CONTOUR_BLACK = 0x01,
        CONTOUR_REFLECT = 0x02
    };

    // battle troop stats
    class Unit : public ArmyTroop, public BitModes, public Control
    {
    public:
        Unit(const Troop&, s32 pos, bool reflect);

        virtual ~Unit();

        bool isModes(uint32_t) const;

        bool isBattle() const;

        string GetShotString() const;

        string GetSpeedString() const;

        uint32_t GetHitPointsLeft() const;

        uint32_t GetAffectedDuration(uint32_t) const;

        uint32_t GetSpeed() const;

        Surface GetContour(int) const;

        void InitContours();

        void SetMirror(Unit*);

        void SetRandomMorale();

        void SetRandomLuck();

        void NewTurn();

        bool isValid() const;

        bool isArchers() const;

        bool isFly() const;

        bool isTwiceAttack() const;

        bool AllowResponse() const;

        bool isHandFighting() const;

        bool isReflect() const;

        bool isHaveDamage() const;

        bool isMagicResist(const Spell&, uint32_t) const;

        bool isMagicAttack() const;

        bool OutOfWalls() const;


        string String(bool more = false) const;

        uint32_t GetUID() const;

        bool isUID(uint32_t) const;

        s32 GetHeadIndex() const;

        s32 GetTailIndex() const;

        const Position& GetPosition() const;

        void SetPosition(s32);

        void SetPosition(const Position&);

        void SetReflection(bool);

        uint32_t GetAttack() const;

        uint32_t GetDefense() const;

        int GetArmyColor() const;

        int GetColor() const;

        uint32_t GetSpeed(bool skip_standing_check) const;

        int GetControl() const;

        uint32_t GetDamage(const Unit&) const;

        s32 GetScoreQuality(const Unit&) const;

        uint32_t GetDead() const;

        uint32_t GetHitPointsTroop() const;

        uint32_t GetShots() const;

        uint32_t ApplyDamage(Unit&, uint32_t);

        uint32_t ApplyDamage(uint32_t);

        uint32_t GetDamageMin(const Unit&) const;

        uint32_t GetDamageMax(const Unit&) const;

        uint32_t CalculateDamageUnit(const Unit&, double) const;

        bool ApplySpell(const Spell&, const HeroBase* hero, TargetInfo&);

        bool AllowApplySpell(const Spell&, const HeroBase* hero, string* msg = nullptr) const;

        void PostAttackAction(Unit&);

        void ResetBlind();

        void SpellModesAction(const Spell&, uint32_t, const HeroBase*);

        void SpellApplyDamage(const Spell&, uint32_t, const HeroBase*, TargetInfo&);

        void SpellRestoreAction(const Spell&, uint32_t, const HeroBase*);

        uint32_t Resurrect(uint32_t, bool, bool);

        const monstersprite_t&
        GetMonsterSprite() const;

        const animframe_t&
        GetFrameState() const;

        const animframe_t&
        GetFrameState(int) const;

        void IncreaseAnimFrame(bool loop = false);

        bool isStartAnimFrame() const;

        bool isFinishAnimFrame() const;

        void SetFrameStep(int);

        void SetFrame(int);

        int GetFrame() const;

        int GetFrameOffset() const;

        int GetFrameStart() const;

        int GetFrameCount() const;

        int GetStartMissileOffset(int) const;

        int M82Attk() const;

        int M82Kill() const;

        int M82Move() const;

        int M82Wnce() const;

        int M82Expl() const;

        int ICNFile() const;

        int ICNMiss() const;

        Point GetBackPoint() const;

        Rect GetRectPosition() const;

        uint32_t HowManyCanKill(const Unit&) const;

        uint32_t HowManyWillKilled(uint32_t&) const;

        void SetResponse();

        void ResetAnimFrame(int);

        void UpdateDirection();

        bool UpdateDirection(const Rect&);

        void PostKilledAction();

        uint32_t GetMagicResist(const Spell&, uint32_t) const;

        int GetSpellMagic(bool force = false) const;

        uint32_t GetObstaclesPenalty(const Unit&) const;

        const HeroBase* GetCommander() const;

        static bool isHandFighting(const Unit&, const Unit&);

    private:
        friend ByteVectorWriter& operator<<(ByteVectorWriter&, const Unit&);

        friend ByteVectorReader& operator>>(ByteVectorReader&, Unit&);

        uint32_t uid;
        uint32_t hp;
        uint32_t count0;
        uint32_t dead;
        uint32_t shots;
        uint32_t disruptingray;
        bool reflect;

        uint32_t animstate;
        s32 animframe;
        s32 animstep;

        Position position;
        ModesAffected affected;
        Unit* mirror;
        Surface contours[4];

        bool blindanswer;
    };

    ByteVectorWriter& operator<<(ByteVectorWriter&, const Unit&);

    ByteVectorReader& operator>>(ByteVectorReader&, Unit&);
}
