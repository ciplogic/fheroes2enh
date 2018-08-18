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
#include <algorithm>

#include <functional>

#include "gamedefs.h"
#include "race.h"
#include "text.h"
#include "agg.h"
#include "game.h"
#include "dialog.h"
#include "dialog_selectitems.h"
#include "settings.h"
#include "skill_static.h"
#include "ByteVectorReader.h"
#include "game_static.h"
#include "rand.h"
#include "icn.h"

namespace Skill
{
    int SecondaryGetWeightSkillFromRace(int race, int skill);

    int SecondaryPriorityFromRace(int, const vector<int>&);

    const int secskills[] = {
        Secondary::PATHFINDING, Secondary::ARCHERY, Secondary::LOGISTICS, Secondary::SCOUTING,
        Secondary::DIPLOMACY, Secondary::NAVIGATION, Secondary::LEADERSHIP, Secondary::WISDOM,
        Secondary::MYSTICISM,
        Secondary::LUCK, Secondary::BALLISTICS, Secondary::EAGLEEYE, Secondary::NECROMANCY,
        Secondary::ESTATES
    };
}

uint32_t Skill::Secondary::GetValues() const
{
    const values_t* val = GameStatic::GetSkillValues(Skill());

    if (val)
        switch (Level())
        {
        case Level::BASIC:
            return val->values.basic;
        case Level::ADVANCED:
            return val->values.advanced;
        case Level::EXPERT:
            return val->values.expert;
        default:
            break;
        }

    return 0;
}

Skill::Primary::Primary() : attack(0), defense(0), power(0), knowledge(0)
{
}

bool Skill::Primary::isCaptain() const
{
    return false;
}

bool Skill::Primary::isHeroes() const
{
    return false;
}

void Skill::Primary::LoadDefaults(int type, int race)
{
    const stats_t* ptr = GameStatic::GetSkillStats(race);

    if (ptr)
        switch (type)
        {
        case HeroBase::CAPTAIN:
            attack = ptr->captain_primary.attack;
            defense = ptr->captain_primary.defense;
            power = ptr->captain_primary.power;
            knowledge = ptr->captain_primary.knowledge;
            break;

        case HeroBase::HEROES:
            attack = ptr->initial_primary.attack;
            defense = ptr->initial_primary.defense;
            power = ptr->initial_primary.power;
            knowledge = ptr->initial_primary.knowledge;
            break;

        default:
            break;
        }
}

int Skill::Primary::GetInitialSpell(int race)
{
    const stats_t* ptr = GameStatic::GetSkillStats(race);
    return ptr ? ptr->initial_spell : 0;
}

void Skill::Primary::ReadFrom(ByteVectorReader& msg)
{
    auto& skill = *this;
    msg >> skill.attack >> skill.defense >> skill.knowledge >> skill.power;
}

int Skill::Primary::LevelUp(int race, int level)
{
    Rand::Queue percents(MAXPRIMARYSKILL);

    const stats_t* ptr = GameStatic::GetSkillStats(race);
    if (ptr)
    {
        if (ptr->over_level > level)
        {
            percents.Push(ATTACK, ptr->mature_primary_under.attack);
            percents.Push(DEFENSE, ptr->mature_primary_under.defense);
            percents.Push(POWER, ptr->mature_primary_under.power);
            percents.Push(KNOWLEDGE, ptr->mature_primary_under.knowledge);
        }
        else
        {
            percents.Push(ATTACK, ptr->mature_primary_over.attack);
            percents.Push(DEFENSE, ptr->mature_primary_over.defense);
            percents.Push(POWER, ptr->mature_primary_over.power);
            percents.Push(KNOWLEDGE, ptr->mature_primary_over.knowledge);
        }
    }

    const int result = percents.Size() ? percents.Get() : UNKNOWN;

    switch (result)
    {
    case ATTACK:
        ++attack;
        break;
    case DEFENSE:
        ++defense;
        break;
    case POWER:
        ++power;
        break;
    case KNOWLEDGE:
        ++knowledge;
        break;
    default:
        break;
    }

    return result;
}

std::string Skill::Primary::String(int skill)
{
    std::string str_skill[] = {_("Attack"), _("Defense"), _("Power"), _("Knowledge"), "Unknown"};

    switch (skill)
    {
    case ATTACK:
        return str_skill[0];
    case DEFENSE:
        return str_skill[1];
    case POWER:
        return str_skill[2];
    case KNOWLEDGE:
        return str_skill[3];
    default:
        break;
    }

    return str_skill[4];
}

string Skill::Primary::StringDescription(int skill, const Heroes* hero)
{
    string res, ext;

    switch (skill)
    {
    case ATTACK:
        res = _("Your attack skill is a bonus added to each creature's attack skill.");
        if (hero) hero->GetAttack(&ext);
        break;

    case DEFENSE:
        res = _("Your defense skill is a bonus added to each creature's defense skill.");
        if (hero) hero->GetDefense(&ext);
        break;

    case POWER:
        res = _("Your spell power determines the length or power of a spell.");
        if (hero) hero->GetPower(&ext);
        break;

    case KNOWLEDGE:
        res = _(
            "Your knowledge determines how many spell points your hero may have. Under normal cirumstances, a hero is limited to 10 spell points per level of knowledge."
        );
        if (hero) hero->GetKnowledge(&ext);
        break;

    default:
        break;
    }

    if (!ext.empty())
    {
        res.append("\n \n");
        res.append(_("Current Modifiers:"));
        res.append("\n \n");
        res.append(ext);
    }

    return res;
}

string Skill::Primary::StringSkills(const string& sep) const
{
    ostringstream os;
    os << Int2Str(attack) << sep << Int2Str(defense) << sep << Int2Str(knowledge) << sep << Int2Str(power);
    return os.str();
}

std::string Skill::Level::String(int level)
{
    std::string str_level[] = {"None", _("skill|Basic"), _("skill|Advanced"), _("skill|Expert")};

    switch (level)
    {
    case BASIC:
        return str_level[1];
    case ADVANCED:
        return str_level[2];
    case EXPERT:
        return str_level[3];
    default:
        break;
    }

    return str_level[0];
}

Skill::Secondary::Secondary() : pair<int, int>(UNKNOWN, Level::NONE)
{
}

Skill::Secondary::Secondary(int skill, int level) : pair<int, int>(UNKNOWN, Level::NONE)
{
    SetSkill(skill);
    SetLevel(level);
}

void Skill::Secondary::Reset()
{
    first = UNKNOWN;
    second = Level::NONE;
}

void Skill::Secondary::Set(const Secondary& skill)
{
    first = skill.first;
    second = skill.second;
}

void Skill::Secondary::SetSkill(int skill)
{
    first = skill <= ESTATES ? skill : UNKNOWN;
}

void Skill::Secondary::SetLevel(int level)
{
    second = level <= Level::EXPERT ? level : Level::NONE;
}

void Skill::Secondary::NextLevel()
{
    switch (second)
    {
    case Level::NONE:
        second = Level::BASIC;
        break;
    case Level::BASIC:
        second = Level::ADVANCED;
        break;
    case Level::ADVANCED:
        second = Level::EXPERT;
        break;
    default:
        break;
    }
}

int Skill::Secondary::Skill() const
{
    return first;
}

int Skill::Secondary::Level() const
{
    return second;
}

bool Skill::Secondary::isLevel(int level) const
{
    return level == second;
}

bool Skill::Secondary::isSkill(int skill) const
{
    return skill == first;
}

bool Skill::Secondary::isValid() const
{
    return Skill() != UNKNOWN && Level() != Level::NONE;
}

int Skill::Secondary::RandForWitchsHut()
{
    const secondary_t* sec = GameStatic::GetSkillForWitchsHut();
    vector<int> v;

    if (sec)
    {
        v.reserve(14);

        if (sec->archery) v.push_back(ARCHERY);
        if (sec->ballistics) v.push_back(BALLISTICS);
        if (sec->diplomacy) v.push_back(DIPLOMACY);
        if (sec->eagleeye) v.push_back(EAGLEEYE);
        if (sec->estates) v.push_back(ESTATES);
        if (sec->leadership) v.push_back(LEADERSHIP);
        if (sec->logistics) v.push_back(LOGISTICS);
        if (sec->luck) v.push_back(LUCK);
        if (sec->mysticism) v.push_back(MYSTICISM);
        if (sec->navigation) v.push_back(NAVIGATION);
        if (sec->necromancy) v.push_back(NECROMANCY);
        if (sec->pathfinding) v.push_back(PATHFINDING);
        if (sec->scouting) v.push_back(SCOUTING);
        if (sec->wisdom) v.push_back(WISDOM);
    }

    return v.empty() ? UNKNOWN : *Rand::Get(v);
}

/* index sprite from SECSKILL */
int Skill::Secondary::GetIndexSprite1() const
{
    return Skill() <= ESTATES ? Skill() : 0;
}

/* index sprite from MINISS */
int Skill::Secondary::GetIndexSprite2() const
{
    return Skill() <= ESTATES ? Skill() - 1 : 0xFF;
}

std::string Skill::Secondary::String(int skill)
{
    std::string str_skill[] = {
        _("Pathfinding"), _("Archery"), _("Logistics"), _("Scouting"), _("Diplomacy"),
        _("Navigation"),
        _("Leadership"), _("Wisdom"), _("Mysticism"), _("Luck"), _("Ballistics"), _("Eagle Eye"),
        _("Necromancy"), _("Estates"), "Unknown"
    };

    switch (skill)
    {
    case PATHFINDING:
        return str_skill[0];
    case ARCHERY:
        return str_skill[1];
    case LOGISTICS:
        return str_skill[2];
    case SCOUTING:
        return str_skill[3];
    case DIPLOMACY:
        return str_skill[4];
    case NAVIGATION:
        return str_skill[5];
    case LEADERSHIP:
        return str_skill[6];
    case WISDOM:
        return str_skill[7];
    case MYSTICISM:
        return str_skill[8];
    case LUCK:
        return str_skill[9];
    case BALLISTICS:
        return str_skill[10];
    case EAGLEEYE:
        return str_skill[11];
    case NECROMANCY:
        return str_skill[12];
    case ESTATES:
        return str_skill[13];

    default:
        break;
    }

    return str_skill[14];
}

std::string Skill::Secondary::GetName() const
{
    std::string name_skill[] =
    {
        _("Basic Pathfinding"), _("Advanced Pathfinding"), _("Expert Pathfinding"),
        _("Basic Archery"), _("Advanced Archery"), _("Expert Archery"),
        _("Basic Logistics"), _("Advanced Logistics"), _("Expert Logistics"),
        _("Basic Scouting"), _("Advanced Scouting"), _("Expert Scouting"),
        _("Basic Diplomacy"), _("Advanced Diplomacy"), _("Expert Diplomacy"),
        _("Basic Navigation"), _("Advanced Navigation"), _("Expert Navigation"),
        _("Basic Leadership"), _("Advanced Leadership"), _("Expert Leadership"),
        _("Basic Wisdom"), _("Advanced Wisdom"), _("Expert Wisdom"),
        _("Basic Mysticism"), _("Advanced Mysticism"), _("Expert Mysticism"),
        _("Basic Luck"), _("Advanced Luck"), _("Expert Luck"),
        _("Basic Ballistics"), _("Advanced Ballistics"), _("Expert Ballistics"),
        _("Basic Eagle Eye"), _("Advanced Eagle Eye"), _("Expert Eagle Eye"),
        _("Basic Necromancy"), _("Advanced Necromancy"), _("Expert Necromancy"),
        _("Basic Estates"), _("Advanced Estates"), _("Expert Estates")
    };

    return isValid() ? name_skill[Level() - 1 + (Skill() - 1) * 3] : "unknown";
}

string Skill::Secondary::GetDescription() const
{
    const uint32_t count = GetValues();
    string str = "unknown";

    switch (Skill())
    {
    case PATHFINDING:
        str = _n("Reduces the movement penalty for rough terrain by %{count} percent.",
            "Reduces the movement penalty for rough terrain by %{count} percent.", count);
        break;
    case ARCHERY:
        str = _n("Increases the damage done by range attacking creatures by %{count} percent.",
            "Increases the damage done by range attacking creatures by %{count} percent.", count);
        break;
    case LOGISTICS:
        str = _n("Increases your hero's movement points by %{count} percent.",
            "Increases your hero's movement points by %{count} percent.", count);
        break;
    case SCOUTING:
        str = _n("Increases your hero's viewable area by %{count} square.",
            "Increases your hero's viewable area by %{count} squares.", count);
        break;
    case DIPLOMACY:
        str = _("Allows you to negotiate with monsters who are weaker than your group.");
        str.append(" ");
        str.append(_n("Approximately %{count} percent of the creatures may offer to join you.",
            "Approximately %{count} percent of the creatures may offer to join you.", count));
        break;
    case NAVIGATION:
        str = _n("Increases your hero's movement points over water by %{count} percent.",
            "Increases your hero's movement points over water by %{count} percent.", count);
        break;
    case LEADERSHIP:
        str = _n("Increases your hero's troops morale by %{count}.",
            "Increases your hero's troops morale by %{count}.", count);
        break;
    case WISDOM:
        {
            switch (Level())
            {
            case Level::BASIC:
                str = _("Allows your hero to learn third level spells.");
                break;
            case Level::ADVANCED:
                str = _("Allows your hero to learn fourth level spells.");
                break;
            case Level::EXPERT:
                str = _("Allows your hero to learn fifth level spells.");
                break;
            default:
                break;
            }
            break;
        }
    case MYSTICISM:
        str = _n("Regenerates %{count} of your hero's spell point per day.",
            "Regenerates %{count} of your hero's spell points per day.", count);
        break;
    case LUCK:
        str = _n("Increases your hero's luck by %{count}.",
            "Increases your hero's luck by %{count}.", count);
        break;
    case BALLISTICS:
        switch (Level())
        {
        case Level::BASIC:
            str = _("Gives your hero's catapult shots a greater chance to hit and do damage to castle walls.");
            break;
        case Level::ADVANCED:
            str = _(
                "Gives your hero's catapult an extra shot, and each shot has a greater chance to hit and do damage to castle walls."
            );
            break;
        case Level::EXPERT:
            str = _(
                "Gives your hero's catapult an extra shot, and each shot automatically destroys any wall, except a fortified wall in a Knight town."
            );
            break;
        default:
            break;
        }
        break;
    case EAGLEEYE:
        switch (Level())
        {
        case Level::BASIC:
            str = _n(
                "Gives your hero a %{count} percent chance to learn any given 1st or 2nd level enemy spell used against him in a combat."
                ,
                "Gives your hero a %{count} percent chance to learn any given 1st or 2nd level enemy spell used against him in a combat."
                ,
                count);
            break;
        case Level::ADVANCED:
            str = _n(
                "Gives your hero a %{count} percent chance to learn any given 3rd level spell (or below) used against him in combat."
                ,
                "Gives your hero a %{count} percent chance to learn any given 3rd level spell (or below) used against him in combat."
                ,
                count);
            break;
        case Level::EXPERT:
            str = _n(
                "Gives your hero a %{count} percent chance to learn any given 4th level spell (or below) used against him in combat."
                ,
                "Gives your hero a %{count} percent chance to learn any given 4th level spell (or below) used against him in combat."
                ,
                count);
            break;
        default:
            break;
        }
        break;
    case NECROMANCY:
        str = _n(
            "Allows %{count} percent of the creatures killed in combat to be brought back from the dead as Skeletons.",
            "Allows %{count} percent of the creatures killed in combat to be brought back from the dead as Skeletons.",
            count);
        break;
    case ESTATES:
        str = _n("Your hero produce %{count} gold pieces per turn as tax revenue from estates.",
            "Your hero produces %{count} gold pieces per turn as tax revenue from estates.", count);
        break;
    default:
        break;
    }

    StringReplace(str, "%{count}", count);

    return str;
}

Skill::SecSkills::SecSkills()
{
    _items.reserve(HEROESMAXSKILL);
}

Skill::SecSkills::SecSkills(int race)
{
    _items.reserve(HEROESMAXSKILL);

    if (race & Race::ALL)
    {
        const stats_t* ptr = GameStatic::GetSkillStats(race);

        if (ptr)
        {
            if (ptr->initial_secondary.archery) AddSkill(Secondary(Secondary::ARCHERY, ptr->initial_secondary.archery));
            if (ptr->initial_secondary.ballistics)
                AddSkill(Secondary(Secondary::BALLISTICS, ptr->initial_secondary.ballistics));
            if (ptr->initial_secondary.diplomacy)
                AddSkill(Secondary(Secondary::DIPLOMACY, ptr->initial_secondary.diplomacy));
            if (ptr->initial_secondary.eagleeye)
                AddSkill(Secondary(Secondary::EAGLEEYE, ptr->initial_secondary.eagleeye));
            if (ptr->initial_secondary.estates) AddSkill(Secondary(Secondary::ESTATES, ptr->initial_secondary.estates));
            if (ptr->initial_secondary.leadership)
                AddSkill(Secondary(Secondary::LEADERSHIP, ptr->initial_secondary.leadership));
            if (ptr->initial_secondary.logistics)
                AddSkill(Secondary(Secondary::LOGISTICS, ptr->initial_secondary.logistics));
            if (ptr->initial_secondary.luck) AddSkill(Secondary(Secondary::LUCK, ptr->initial_secondary.luck));
            if (ptr->initial_secondary.mysticism)
                AddSkill(Secondary(Secondary::MYSTICISM, ptr->initial_secondary.mysticism));
            if (ptr->initial_secondary.navigation)
                AddSkill(Secondary(Secondary::NAVIGATION, ptr->initial_secondary.navigation));
            if (ptr->initial_secondary.necromancy)
                AddSkill(Secondary(Secondary::NECROMANCY, ptr->initial_secondary.necromancy));
            if (ptr->initial_secondary.pathfinding)
                AddSkill(Secondary(Secondary::PATHFINDING, ptr->initial_secondary.pathfinding));
            if (ptr->initial_secondary.scouting)
                AddSkill(Secondary(Secondary::SCOUTING, ptr->initial_secondary.scouting));
            if (ptr->initial_secondary.wisdom) AddSkill(Secondary(Secondary::WISDOM, ptr->initial_secondary.wisdom));
        }
    }
}

int Skill::SecSkills::GetLevel(int skill) const
{
    const auto it = find_if(_items.begin(), _items.end(),
                            bind2nd(mem_fun_ref(&Secondary::isSkill), skill));

    return it == _items.end() ? Level::NONE : (*it).Level();
}

uint32_t Skill::SecSkills::GetValues(int skill) const
{
    const auto it = find_if(_items.begin(), _items.end(),
                            bind2nd(mem_fun_ref(&Secondary::isSkill), skill));

    return it == _items.end() ? 0 : (*it).GetValues();
}

int Skill::SecSkills::Count() const
{
    return count_if(_items.begin(), _items.end(), mem_fun_ref(&Secondary::isValid));
}

void Skill::SecSkills::AddSkill(const Secondary& skill)
{
    if (skill.isValid())
    {
        auto it = find_if(_items.begin(), _items.end(),
                          bind2nd(mem_fun_ref(&Secondary::isSkill), skill.Skill()));
        if (it != _items.end())
            (*it).SetLevel(skill.Level());
        else
        {
            it = find_if(_items.begin(), _items.end(),
                         not1(mem_fun_ref(&Secondary::isValid)));
            if (it != _items.end())
                (*it).Set(skill);
            else if (_items.size() < HEROESMAXSKILL)
                _items.push_back(skill);
        }
    }
}

Skill::Secondary* Skill::SecSkills::FindSkill(int skill)
{
    const auto it = find_if(_items.begin(), _items.end(),
                            bind2nd(mem_fun_ref(&Secondary::isSkill), skill));
    return it != _items.end() ? &*it : nullptr;
}

vector<Skill::Secondary>& Skill::SecSkills::ToVector()
{
    vector<Secondary>& v = _items;
    return v;
}

string Skill::SecSkills::String() const
{
    ostringstream os;

    for (const auto& it : _items)
        os << it.GetName() << ", ";

    return os.str();
}

void Skill::SecSkills::FillMax(const Secondary& skill)
{
    if (_items.size() < HEROESMAXSKILL)
        _items.resize(HEROESMAXSKILL, skill);
}

int Skill::SecondaryGetWeightSkillFromRace(int race, int skill)
{
    const stats_t* ptr = GameStatic::GetSkillStats(race);

    if (!ptr)
        return 0;
    if (skill == Secondary::PATHFINDING) return ptr->mature_secondary.pathfinding;
    if (skill == Secondary::ARCHERY) return ptr->mature_secondary.archery;
    if (skill == Secondary::LOGISTICS) return ptr->mature_secondary.logistics;
    if (skill == Secondary::SCOUTING) return ptr->mature_secondary.scouting;
    if (skill == Secondary::DIPLOMACY) return ptr->mature_secondary.diplomacy;
    if (skill == Secondary::NAVIGATION) return ptr->mature_secondary.navigation;
    if (skill == Secondary::LEADERSHIP) return ptr->mature_secondary.leadership;
    if (skill == Secondary::WISDOM) return ptr->mature_secondary.wisdom;
    if (skill == Secondary::MYSTICISM) return ptr->mature_secondary.mysticism;
    if (skill == Secondary::LUCK) return ptr->mature_secondary.luck;
    if (skill == Secondary::BALLISTICS) return ptr->mature_secondary.ballistics;
    if (skill == Secondary::EAGLEEYE) return ptr->mature_secondary.eagleeye;
    if (skill == Secondary::NECROMANCY) return ptr->mature_secondary.necromancy;
    if (skill == Secondary::ESTATES) return ptr->mature_secondary.estates;

    return 0;
}

/*
std::vector<int> Skill::SecondarySkills()
{
    const int vals[] = { Secondary::PATHFINDING, Secondary::ARCHERY, Secondary::LOGISTICS, Secondary::SCOUTING,
			Secondary::DIPLOMACY, Secondary::NAVIGATION, Secondary::LEADERSHIP, Secondary::WISDOM, Secondary::MYSTICISM,
			Secondary::LUCK, Secondary::BALLISTICS, Secondary::EAGLEEYE, Secondary::NECROMANCY, Secondary::ESTATES };

    return std::vector<int>(vals, ARRAY_COUNT_END(vals));
}
*/

int Skill::SecondaryPriorityFromRace(int race, const vector<int>& exclude)
{
    Rand::Queue parts(MAXSECONDARYSKILL);

    for (uint32_t ii = 0; ii < ARRAY_COUNT(secskills); ++ii)
        if (exclude.empty() ||
            exclude.end() == find(exclude.begin(), exclude.end(), secskills[ii]))
            parts.Push(secskills[ii], SecondaryGetWeightSkillFromRace(race, secskills[ii]));

    return parts.Size() ? parts.Get() : Secondary::UNKNOWN;
}

/* select secondary skills for level up */
void Skill::SecSkills::FindSkillsForLevelUp(int race, Secondary& sec1, Secondary& sec2) const
{
    vector<int> exclude_skills;
    exclude_skills.reserve(MAXSECONDARYSKILL + HEROESMAXSKILL);

    // exclude for expert
    for (const auto& it : _items)
        if (it.Level() == Level::EXPERT) exclude_skills.push_back(it.Skill());

    // exclude is full, add other.
    if (HEROESMAXSKILL <= Count())
    {
        for (uint32_t ii = 0; ii < ARRAY_COUNT(secskills); ++ii)
            if (Level::NONE == GetLevel(secskills[ii])) exclude_skills.push_back(secskills[ii]);
    }

    sec1.SetSkill(SecondaryPriorityFromRace(race, exclude_skills));

    if (Secondary::UNKNOWN != sec1.Skill())
    {
        exclude_skills.push_back(sec1.Skill());
        sec2.SetSkill(SecondaryPriorityFromRace(race, exclude_skills));

        sec1.SetLevel(GetLevel(sec1.Skill()));
        sec2.SetLevel(GetLevel(sec2.Skill()));

        sec1.NextLevel();
        sec2.NextLevel();
    }
    else if (Settings::Get().ExtHeroAllowBannedSecSkillsUpgrade())
    {
        auto it = find_if(_items.begin(), _items.end(),
                          [](auto& skill)
                          {
                              return !skill.isLevel(static_cast<int>(Level::EXPERT));
                          });
        if (it != _items.end())
        {
            sec1.SetSkill((*it).Skill());
            sec1.SetLevel(GetLevel(sec1.Skill()));
            sec1.NextLevel();
        }
    }
}

void StringAppendModifiers(string& str, int value)
{
    if (value < 0) str.append(" "); // '-' present
    else if (value > 0) str.append(" +");

    str.append(Int2Str(value));
}

int Skill::GetLeadershipModifiers(int level, string* strs = nullptr)
{
    Secondary skill(Secondary::LEADERSHIP, level);

    if (skill.GetValues() && strs)
    {
        strs->append(skill.GetName());
        StringAppendModifiers(*strs, skill.GetValues());
        strs->append("\n");
    }

    return skill.GetValues();
}

int Skill::GetLuckModifiers(int level, string* strs = nullptr)
{
    Secondary skill(Secondary::LUCK, level);

    if (skill.GetValues() && strs)
    {
        strs->append(skill.GetName());
        StringAppendModifiers(*strs, skill.GetValues());
        strs->append("\n");
    }

    return skill.GetValues();
}

ByteVectorWriter& Skill::operator<<(ByteVectorWriter& msg, const Primary& skill)
{
    return msg << skill.attack << skill.defense << skill.knowledge << skill.power;
}


ByteVectorReader& Skill::operator>>(ByteVectorReader& msg, Primary& skill)
{
    return msg >> skill.attack >> skill.defense >> skill.knowledge >> skill.power;
}

Surface GetBarBackgroundSprite()
{
    const Rect rt(26, 21, 32, 32);
    Surface res(Size(rt.w, rt.h) + Size(2, 2), true);
    res.DrawBorder(RGBA(0xD0, 0xC0, 0x48));
    AGG::GetICN(ICN::HSICONS, 0).Blit(rt, 1, 1, res);
    return res;
}

PrimarySkillsBar::PrimarySkillsBar(const Heroes* hr, bool mini) : hero(hr), use_mini_sprite(mini), toff(0, 0)
{
    content.push_back(Skill::Primary::ATTACK);
    content.push_back(Skill::Primary::DEFENSE);
    content.push_back(Skill::Primary::POWER);
    content.push_back(Skill::Primary::KNOWLEDGE);

    if (use_mini_sprite)
    {
        backsf = GetBarBackgroundSprite();
        SetItemSize(backsf.w(), backsf.h());
    }
    else
    {
        const Sprite& sprite = AGG::GetICN(ICN::PRIMSKIL, 0);
        SetItemSize(sprite.w(), sprite.h());
    }

    SetContent(content);
}

void PrimarySkillsBar::SetTextOff(s32 ox, s32 oy)
{
    toff = Point(ox, oy);
}

void PrimarySkillsBar::RedrawBackground(const Rect& pos, Surface& dstsf)
{
    if (use_mini_sprite)
        backsf.Blit(pos, dstsf);
}

void PrimarySkillsBar::RedrawItem(int& skill, const Rect& pos, Surface& dstsf)
{
    if (Skill::Primary::UNKNOWN == skill)
        return;
    if (use_mini_sprite)
    {
        const Sprite& backSprite = AGG::GetICN(ICN::SWAPWIN, 0);
        const int ww = 32;
        Text text("", Font::SMALL);
        const Point dstpt(pos.x + (pos.w - ww) / 2, pos.y + (pos.h - ww) / 2);

        switch (skill)
        {
        case Skill::Primary::ATTACK:
            backSprite.Blit(Rect(217, 52, ww, ww), dstpt, dstsf);
            if (hero) text.Set(Int2Str(hero->GetAttack()));
            break;

        case Skill::Primary::DEFENSE:
            backSprite.Blit(Rect(217, 85, ww, ww), dstpt, dstsf);
            if (hero) text.Set(Int2Str(hero->GetDefense()));
            break;

        case Skill::Primary::POWER:
            backSprite.Blit(Rect(217, 118, ww, ww), dstpt, dstsf);
            if (hero) text.Set(Int2Str(hero->GetPower()));
            break;

        case Skill::Primary::KNOWLEDGE:
            backSprite.Blit(Rect(217, 151, ww, ww), dstpt, dstsf);
            if (hero) text.Set(Int2Str(hero->GetKnowledge()));
            break;

        default:
            break;
        }

        if (hero) text.Blit(pos.x + (pos.w + toff.x - text.w()) / 2, pos.y + pos.h + toff.y, dstsf);
    }
    else
    {
        const Sprite& sprite = AGG::GetICN(ICN::PRIMSKIL, skill - 1);
        sprite.Blit(pos.x + (pos.w - sprite.w()) / 2, pos.y + (pos.h - sprite.h()) / 2, dstsf);

        Text text(Skill::Primary::String(skill), Font::SMALL);
        text.Blit(pos.x + (pos.w - text.w()) / 2, pos.y + 3, dstsf);

        if (hero)
        {
            switch (skill)
            {
            case Skill::Primary::ATTACK:
                text.Set(Int2Str(hero->GetAttack()), Font::BIG);
                break;

            case Skill::Primary::DEFENSE:
                text.Set(Int2Str(hero->GetDefense()), Font::BIG);
                break;

            case Skill::Primary::POWER:
                text.Set(Int2Str(hero->GetPower()), Font::BIG);
                break;

            case Skill::Primary::KNOWLEDGE:
                text.Set(Int2Str(hero->GetKnowledge()), Font::BIG);
                break;

            default:
                break;
            }

            text.Blit(pos.x + (pos.w - text.w()) / 2, pos.y + pos.h - text.h() - 3, dstsf);
        }
    }
}

bool PrimarySkillsBar::ActionBarSingleClick(const Point& cursor, int& skill, const Rect& pos)
{
    if (Skill::Primary::UNKNOWN == skill)
        return false;
    Message(Skill::Primary::String(skill), Skill::Primary::StringDescription(skill, hero), Font::BIG,
            Dialog::OK);
    return true;
}

bool PrimarySkillsBar::ActionBarPressRight(const Point& cursor, int& skill, const Rect& pos)
{
    if (Skill::Primary::UNKNOWN == skill)
        return false;
    Dialog::Message(Skill::Primary::String(skill), Skill::Primary::StringDescription(skill, hero), Font::BIG);
    return true;
}

bool PrimarySkillsBar::ActionBarCursor(const Point& cursor, int& skill, const Rect& pos)
{
    if (Skill::Primary::UNKNOWN != skill)
    {
        msg = _("View %{skill} Info");
        StringReplace(msg, "%{skill}", Skill::Primary::String(skill));
    }

    return false;
}

bool PrimarySkillsBar::QueueEventProcessing(string* str)
{
    msg.clear();
    const bool res = ItemsBar<int>::QueueEventProcessing();
    if (str) *str = msg;
    return res;
}

SecondarySkillsBar::SecondarySkillsBar(bool mini /* true */, bool change /* false */) : use_mini_sprite(mini),
                                                                                        can_change(change)
{
    if (use_mini_sprite)
    {
        backsf = GetBarBackgroundSprite();
        SetItemSize(backsf.w(), backsf.h());
    }
    else
    {
        const Sprite& sprite = AGG::GetICN(ICN::SECSKILL, 0);
        SetItemSize(sprite.w(), sprite.h());
    }
}

void SecondarySkillsBar::RedrawBackground(const Rect& pos, Surface& dstsf)
{
    if (use_mini_sprite)
        backsf.Blit(pos, dstsf);
    else
        AGG::GetICN(ICN::SECSKILL, 0).Blit(pos, dstsf);
}

void SecondarySkillsBar::RedrawItem(Skill::Secondary& skill, const Rect& pos, Surface& dstsf)
{
    if (!skill.isValid())
        return;
    const Sprite& sprite = use_mini_sprite
                               ? AGG::GetICN(ICN::MINISS, skill.GetIndexSprite2())
                               : AGG::GetICN(ICN::SECSKILL,
                                             skill.GetIndexSprite1());
    sprite.Blit(pos.x + (pos.w - sprite.w()) / 2, pos.y + (pos.h - sprite.h()) / 2, dstsf);

    if (use_mini_sprite)
    {
        Text text(Int2Str(skill.Level()), Font::SMALL);
        text.Blit(pos.x + (pos.w - text.w()) - 3, pos.y + pos.h - 12, dstsf);
    }
    else
    {
        Text text(Skill::Secondary::String(skill.Skill()), Font::SMALL);
        text.Blit(pos.x + (pos.w - text.w()) / 2, pos.y + 3, dstsf);

        text.Set(Skill::Level::String(skill.Level()));
        text.Blit(pos.x + (pos.w - text.w()) / 2, pos.y + 50, dstsf);
    }
}

bool SecondarySkillsBar::ActionBarSingleClick(const Point& cursor, Skill::Secondary& skill, const Rect& pos)
{
    if (skill.isValid())
    {
        Dialog::SecondarySkillInfo(skill, true);
        return true;
    }
    if (!can_change)
    {
        return false;
    }
    Skill::Secondary alt = Dialog::SelectSecondarySkill();

    if (alt.isValid())
    {
        skill = alt;
        return true;
    }

    return false;
}

bool SecondarySkillsBar::ActionBarPressRight(const Point& cursor, Skill::Secondary& skill, const Rect& pos)
{
    if (!skill.isValid())
        return false;
    if (can_change)
        skill.Reset();
    else
        Dialog::SecondarySkillInfo(skill, false);
    return true;
}

bool SecondarySkillsBar::ActionBarCursor(const Point& cursor, Skill::Secondary& skill, const Rect& pos)
{
    if (skill.isValid())
    {
        msg = _("View %{skill} Info");
        StringReplace(msg, "%{skill}", skill.GetName());
    }

    return false;
}

bool SecondarySkillsBar::QueueEventProcessing(string* str)
{
    msg.clear();
    const bool res = ItemsBar<Skill::Secondary>::QueueEventProcessing();
    if (str) *str = msg;
    return res;
}

ByteVectorReader& Skill::operator>>(ByteVectorReader& sb, Secondary& st)
{
    return sb >> st.first >> st.second;
}

ByteVectorWriter& Skill::operator<<(ByteVectorWriter& sb, const SecSkills& ss)
{
    const vector<Secondary>& v = ss._items;
    return sb << v;
}

ByteVectorReader& Skill::operator>>(ByteVectorReader& sb, SecSkills& ss)
{
    vector<Secondary>& v = ss._items;
    sb >> v;

    if (FORMAT_VERSION_3255 > Game::GetLoadVersion())
    {
        if (v.size() > HEROESMAXSKILL)
            v.resize(HEROESMAXSKILL);
    }
    return sb;
}
