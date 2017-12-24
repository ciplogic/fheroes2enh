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

#ifndef H2SKILL_H
#define H2SKILL_H

#include <string>
#include <vector>
#include <utility>
#include "gamedefs.h"
#include "serialize.h"

#include "ByteVectorReader.h"

void StringAppendModifiers(string &, int);

class Spell;

class Heroes;

namespace Skill
{
    int GetLeadershipModifiers(int level, string *strs);

    int GetLuckModifiers(int level, string *strs);

    void UpdateStats(const string &);

    namespace Level
    {
        enum
        {
            NONE = 0, BASIC = 1, ADVANCED = 2, EXPERT = 3
        };

        const char *String(int level);
    }

    class Secondary : public pair<int, int>
    {
    public:
        enum
        {
            UNKNOWN = 0,
            PATHFINDING = 1,
            ARCHERY = 2,
            LOGISTICS = 3,
            SCOUTING = 4,
            DIPLOMACY = 5,
            NAVIGATION = 6,
            LEADERSHIP = 7,
            WISDOM = 8,
            MYSTICISM = 9,
            LUCK = 10,
            BALLISTICS = 11,
            EAGLEEYE = 12,
            NECROMANCY = 13,
            ESTATES = 14,

            LEARNING = EAGLEEYE
        };

        Secondary();

        Secondary(int skill, int level);

        void Reset();

        void Set(const Secondary &);

        void SetSkill(int);

        void SetLevel(int);

        void NextLevel();

        int Level() const;

        int Skill() const;

        bool isLevel(int) const;

        bool isSkill(int) const;

        bool isValid() const;

        const char *GetName() const;

        string GetDescription() const;

        u32 GetValues() const;

        /* index sprite from SECSKILL */
        int GetIndexSprite1() const;

        /* index sprite from MINISS */
        int GetIndexSprite2() const;

        static int RandForWitchsHut();

        static const char *
        String(int);
    };

    StreamBase &operator>>(StreamBase &, Secondary &);
	ByteVectorReader &operator>>(ByteVectorReader &, Secondary &);

    class SecSkills : protected vector<Secondary>
    {
    public:
        SecSkills();

        SecSkills(int race);

        int GetLevel(int skill) const;

        u32 GetValues(int skill) const;

        void AddSkill(const Secondary &);

        void FindSkillsForLevelUp(int race, Secondary &, Secondary &) const;

        void FillMax(const Secondary &);

        Secondary *FindSkill(int);

        string String() const;

        int Count() const;

        vector<Secondary> &
        ToVector();

    protected:
        friend StreamBase &operator<<(StreamBase &, const SecSkills &);

        friend StreamBase &operator>>(StreamBase &, SecSkills &);
		friend ByteVectorReader &operator>>(ByteVectorReader &, SecSkills &);
    };

    StreamBase &operator<<(StreamBase &, const SecSkills &);

    StreamBase &operator>>(StreamBase &, SecSkills &);
	ByteVectorReader &operator>>(ByteVectorReader &, SecSkills &);

    class Primary
    {
    public:
        Primary();

        virtual ~Primary()
        {};

        enum
        {
            UNKNOWN = 0, ATTACK = 1, DEFENSE = 2, POWER = 3, KNOWLEDGE = 4
        };

        virtual int GetAttack() const = 0;

        virtual int GetDefense() const = 0;

        virtual int GetPower() const = 0;

        virtual int GetKnowledge() const = 0;

        virtual int GetMorale() const = 0;

        virtual int GetLuck() const = 0;

        virtual int GetRace() const = 0;

        virtual bool isCaptain() const;

        virtual bool isHeroes() const;

        int LevelUp(int race, int level);

        string StringSkills(const string &) const;

        static const char *String(int);

        static string StringDescription(int, const Heroes *);

        static int GetInitialSpell(int race);

		void ReadFrom(ByteVectorReader & msg);

    protected:
        void LoadDefaults(int type, int race);

        friend StreamBase &operator<<(StreamBase &, const Primary &);

        friend StreamBase &operator>>(StreamBase &, Primary &);

        int attack;
        int defense;
        int power;
        int knowledge;
    };

    StreamBase &operator<<(StreamBase &, const Primary &);

    StreamBase &operator>>(StreamBase &, Primary &);
}

#include "interface_itemsbar.h"

class PrimarySkillsBar : public Interface::ItemsBar<int>
{
public:
    PrimarySkillsBar(const Heroes *, bool mini);

    void SetTextOff(s32, s32);

    void RedrawBackground(const Rect &, Surface &);

    void RedrawItem(int &, const Rect &, Surface &);

    bool ActionBarSingleClick(const Point &, int &, const Rect &);

    bool ActionBarPressRight(const Point &, int &, const Rect &);

    bool ActionBarCursor(const Point &, int &, const Rect &);

    bool QueueEventProcessing(string * = nullptr);

protected:
    const Heroes *hero;
    Surface backsf;
    bool use_mini_sprite;
    vector<int> content;
    Point toff;
    string msg;
};

class SecondarySkillsBar : public Interface::ItemsBar<Skill::Secondary>
{
public:
    SecondarySkillsBar(bool mini = true, bool change = false);

    void RedrawBackground(const Rect &, Surface &);

    void RedrawItem(Skill::Secondary &, const Rect &, Surface &);

    bool ActionBarSingleClick(const Point &, Skill::Secondary &, const Rect &);

    bool ActionBarPressRight(const Point &, Skill::Secondary &, const Rect &);

    bool ActionBarCursor(const Point &, Skill::Secondary &, const Rect &);

    bool QueueEventProcessing(string * = nullptr);

protected:
    Surface backsf;
    bool use_mini_sprite;
    bool can_change;
    string msg;
};

#endif
