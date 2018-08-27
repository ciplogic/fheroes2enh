/********************************************************************************
 *   Copyright (C) 2010 by Andrey Afletdinov <fheroes2@gmail.com>               *
 *   All rights reserved.                                                       *
 *                                                                              *
 *   Part of the Free Heroes2 Engine:                                           *
 *   http://sourceforge.net/projects/fheroes2                                   *
 *                                                                              *
 *   Redistribution and use in source and binary forms, with or without         *
 *   modification, are permitted provided that the following conditions         *
 *   are met:                                                                   *
 *   - Redistributions may not be sold, nor may they be used in a               *
 *     commercial product or activity.                                          *
 *   - Redistributions of source code and/or in binary form must reproduce      *
 *     the above copyright notice, this list of conditions and the              *
 *     following disclaimer in the documentation and/or other materials         *
 *     provided with the distribution.                                          *
 *                                                                              *
 * THIS SOFTWARE IS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,   *
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS    *
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT     *
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,        *
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;  *
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,     *
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE         *
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,            *
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                           *
 *******************************************************************************/

#include <functional>
#include <algorithm>

#include "settings.h"
#include "kingdom.h"
#include "difficulty.h"
#include "world.h"
#include "game_interface.h"
#include "ai_simple.h"
#include "rand.h"

#include <sstream>
#include "ai.h"

#define HERO_MAX_SHEDULED_TASK 7

AIHeroes& AIHeroes::Get()
{
    static AIHeroes ai_heroes;
    return ai_heroes;
}

AIHero& AIHeroes::Get(const Heroes& ht)
{
    return Get()._items.at(ht.GetID());
}

void AIHeroes::Reset()
{
    AIHeroes& ai = Get();
    for_each(ai._items.begin(), ai._items.end(), std::function<void(AIHero)>(&AIHero::Reset));
}

void AIHero::Reset()
{
    primary_target = -1;
    sheduled_visit.clear();
    fix_loop = 0;
}

bool AI::HeroesSkipFog()
{
    return false;
}

void AI::HeroesActionComplete(Heroes&, s32)
{
}

string AI::HeroesString(const Heroes& hero)
{
    ostringstream os;

    AIHero& ai_hero = AIHeroes::Get(hero);
    Queue& task = ai_hero.sheduled_visit;

    os << "flags           : " <<
        (hero.Modes(HEROES_SCOUTER) ? "SCOUTER," : "") <<
        (hero.Modes(HEROES_HUNTER) ? "HUNTER," : "") <<
        (hero.Modes(HEROES_WAITING) ? "WAITING," : "") <<
        (hero.Modes(HEROES_STUPID) ? "STUPID" : "") << endl;

    os << "ai primary target: " << ai_hero.primary_target << endl <<
        "ai sheduled visit: ";
    for (auto& it : task)
        os << it << "(" << MP2::StringObject(world.GetTiles(it).GetObject()) << "), ";
    os << endl;

    return os.str();
}

void AI::HeroesPostLoad(Heroes& hero)
{
    hero.SetModes(HEROES_HUNTER);
}

void AI::HeroesLevelUp(Heroes& hero)
{
    if (4 < hero.GetLevel() && !hero.Modes(HEROES_HUNTER))
        hero.SetModes(HEROES_HUNTER);

    if (9 < hero.GetLevel() && hero.Modes(HEROES_SCOUTER))
        hero.ResetModes(HEROES_SCOUTER);
}

void AI::HeroesPreBattle(HeroBase& hero)
{
    Castle* castle = world.GetCastle(hero.GetCenter());
    if (castle && hero.GetType() != HeroBase::CAPTAIN)
        hero.GetArmy().m_troops.JoinTroops(castle->GetArmy().m_troops);
}

void AI::HeroesAfterBattle(HeroBase& hero)
{
}

void AI::HeroesClearTask(const Heroes& hero)
{
    AIHeroes::Get(hero).ClearTasks();
}

bool AIHeroesValidObject2(const Heroes* hero, s32 index)
{
    const Heroes& hero2 = *hero;
    return AI::HeroesValidObject(hero2, index);
}

// get priority object for AI independent of distance (1 day)
bool AIHeroesPriorityObject(const Heroes& hero, s32 index)
{
    Maps::Tiles& tile = world.GetTiles(index);

    if (MP2::OBJ_CASTLE == tile.GetObject())
    {
        const Castle* castle = world.GetCastle(Maps::GetPoint(index));
        if (castle)
        {
            if (hero.GetColor() == castle->GetColor())
            {
                // maybe need join army
                return hero.Modes(AI::HEROES_HUNTER) &&
                    castle->GetArmy().m_troops.isValid() &&
                    !hero.isVisited(world.GetTiles(castle->GetIndex()));
            }
            if (!hero.isFriends(castle->GetColor()))
                return AI::HeroesValidObject(hero, index);
        }
    }
    else if (MP2::OBJ_HEROES == tile.GetObject())
    {
        // kill enemy hero
        const Heroes* hero2 = tile.GetHeroes();
        return hero2 &&
            !hero.isFriends(hero2->GetColor()) &&
            AI::HeroesValidObject(hero, index);
    }

    switch (tile.GetObject())
    {
    case MP2::OBJ_MONSTER:
    case MP2::OBJ_SAWMILL:
    case MP2::OBJ_MINES:
    case MP2::OBJ_ALCHEMYLAB:

    case MP2::OBJ_ARTIFACT:
    case MP2::OBJ_RESOURCE:
    case MP2::OBJ_CAMPFIRE:
    case MP2::OBJ_TREASURECHEST:

        return AI::HeroesValidObject(hero, index);

    default:
        break;
    }

    return false;
}

s32 FindUncharteredTerritory(Heroes& hero, uint32_t scoute)
{
    Maps::Indexes v;
    Maps::GetAroundIndexes(hero.GetIndex(), scoute, true, v);
    Maps::Indexes res;

	v.resize(distance(v.begin(),
		remove_if(v.begin(), v.end(),
			[](s32 it) {return Maps::TileIsUnderProtection(it); })));


    for (auto it = v.rbegin(); it != v.rend() && res.size() < 4; ++it)
    {
        // find fogs
        if (world.GetTiles(*it).isFog(hero.GetColor()) &&
            world.GetTiles(*it).isPassable(&hero, Direction::CENTER, true) &&
            hero.GetPath().Calculate(*it))
            res.push_back(*it);
    }

    const s32 result = !res.empty() ? *Rand::Get(res) : -1;

    return result;
}

s32 GetRandomHeroesPosition(Heroes& hero, uint32_t scoute)
{
    Maps::Indexes v;
    Maps::GetAroundIndexes(hero.GetIndex(), scoute, true, v);
    Maps::Indexes res;

    v.resize(distance(v.begin(),
                      remove_if(v.begin(), v.end(), 
						  [](s32 it) {return Maps::TileIsUnderProtection(it); })));


    for (auto it = v.rbegin(); it != v.rend() && res.size() < 4; ++it)
    {
        if (world.GetTiles(*it).isPassable(&hero, Direction::CENTER, true) &&
            hero.GetPath().Calculate(*it))
            res.push_back(*it);
    }

    const s32 result = !res.empty() ? *Rand::Get(res) : -1;

    return result;
}

void AIHeroesAddedRescueTask(Heroes& hero)
{
    AIHero& ai_hero = AIHeroes::Get(hero);
    Queue& task = ai_hero.sheduled_visit;

    uint32_t scoute = hero.GetScoute();

    switch ((DifficultyEnum)Settings::Get().GameDifficulty())
    {
    case DifficultyEnum::NORMAL:
        scoute += 2;
        break;
    case DifficultyEnum::HARD:
        scoute += 3;
        break;
    case DifficultyEnum::EXPERT:
        scoute += 4;
        break;
    case DifficultyEnum::IMPOSSIBLE:
        scoute += 6;
        break;
    default:
        break;
    }

    // find unchartered territory
    s32 index = FindUncharteredTerritory(hero, scoute);
    const Maps::Tiles& tile = world.GetTiles(hero.GetIndex());

    if (index < 0)
    {
        // check teleports
        if (MP2::OBJ_STONELIGHTS == tile.GetObject(false) ||
            MP2::OBJ_WHIRLPOOL == tile.GetObject(false))
        {
            AI::HeroesAction(hero, hero.GetIndex());
        }
        else
        {
            // random
            index = GetRandomHeroesPosition(hero, scoute);
        }
    }

    if (0 <= index) task.push_back(index);
}

void AIHeroesAddedTask(Heroes& hero)
{
    AIHero& ai_hero = AIHeroes::Get(hero);
    AIKingdom& ai_kingdom = AIKingdoms::Get(hero.GetColor());

    Queue& task = ai_hero.sheduled_visit;
    IndexObjectMap& ai_objects = ai_kingdom.scans;

    // load minimal distance tasks
    vector<IndexDistance> objs;
    objs.reserve(ai_objects.size());

    for (auto it = ai_objects.begin(); it != ai_objects.end(); ++it)
    {
        const Maps::Tiles& tile = world.GetTiles((*it).first);

        if (hero.isShipMaster())
        {
            if (MP2::OBJ_COAST != tile.GetObject() &&
                !tile.isWater())
                continue;

            // check previous positions
            if (MP2::OBJ_COAST == (*it).second &&
                hero.isVisited(world.GetTiles((*it).first)))
                continue;
        }
        else
        {
            if (tile.isWater() && MP2::OBJ_BOAT != tile.GetObject()) continue;
        }

        objs.emplace_back((*it).first,
                          Maps::GetApproximateDistance(hero.GetIndex(), (*it).first));
    }


    sort(objs.begin(), objs.end(), IndexDistance::Shortest);

    for (vector<IndexDistance>::const_iterator
         it = objs.begin(); it != objs.end(); ++it)
    {
        if (task.size() >= HERO_MAX_SHEDULED_TASK) break;
        const bool validobj = AI::HeroesValidObject(hero, (*it).first);

        if (validobj &&
            hero.GetPath().Calculate((*it).first))
        {
            task.push_back((*it).first);
            ai_objects.erase((*it).first);
        }
        else
        {
        }
    }

    if (task.empty())
        AIHeroesAddedRescueTask(hero);
}

void AI::HeroesActionNewPosition(Heroes& hero)
{
    AIHero& ai_hero = AIHeroes::Get(hero);
    //AIKingdom & ai_kingdom = AIKingdoms::Get(hero.GetColor());
    Queue& task = ai_hero.sheduled_visit;

    const u8 objs[] = {MP2::OBJ_ARTIFACT, MP2::OBJ_RESOURCE, MP2::OBJ_CAMPFIRE, MP2::OBJ_TREASURECHEST, 0};
    Maps::Indexes pickups = Maps::ScanAroundObjects(hero.GetIndex(), objs);

    if (!pickups.empty() && hero.GetPath().isValid() &&
        pickups.end() == find(pickups.begin(), pickups.end(), hero.GetPath().GetDestinationIndex()))
        hero.GetPath().Reset();

    for (auto& pickup : pickups)
        if (pickup != hero.GetPath().GetDestinationIndex())
            task.push_front(pickup);
}

bool AI::HeroesGetTask(Heroes& hero)
{
    vector<s32> results;
    results.reserve(5);

    const Settings& conf = Settings::Get();
    AIHero& ai_hero = AIHeroes::Get(hero);
    AIKingdom& ai_kingdom = AIKingdoms::Get(hero.GetColor());

    Queue& task = ai_hero.sheduled_visit;
    IndexObjectMap& ai_objects = ai_kingdom.scans;

    const u8 objs1[] = {MP2::OBJ_ARTIFACT, MP2::OBJ_RESOURCE, MP2::OBJ_CAMPFIRE, MP2::OBJ_TREASURECHEST, 0};
    const u8 objs2[] = {MP2::OBJ_SAWMILL, MP2::OBJ_MINES, MP2::OBJ_ALCHEMYLAB, 0};
    const u8 objs3[] = {MP2::OBJ_CASTLE, MP2::OBJ_HEROES, MP2::OBJ_MONSTER, 0};

    // rescan path
    hero.RescanPath();

    Castle* castle = hero.inCastle();
    // if hero in castle
    if (castle)
    {
        castle->RecruitAllMonster();
        hero.GetArmy().m_troops.UpgradeTroops(*castle);

        // recruit army
        if (hero.Modes(HEROES_HUNTER))
            hero.GetArmy().JoinStrongestFromArmy(castle->GetArmy());
        else if (hero.Modes(HEROES_SCOUTER))
            hero.GetArmy().KeepOnlyWeakestTroops(castle->GetArmy());
    }

    // patrol task
    if (hero.Modes(Heroes::PATROL))
    {
        // goto patrol center
        if (hero.GetCenterPatrol() != hero.GetCenter() &&
            hero.GetPath().Calculate(Maps::GetIndexFromAbsPoint(hero.GetCenterPatrol())))
            return true;

        // scan enemy hero
        if (hero.GetSquarePatrol())
        {
            const Maps::Indexes& indicesAround = Maps::ScanAroundObject(Maps::GetIndexFromAbsPoint(hero.GetCenterPatrol()),
                                                                  hero.GetSquarePatrol(), MP2::OBJ_HEROES);
            for (int result : indicesAround)
            {
                const Heroes* enemy = world.GetTiles(result).GetHeroes();
                if (!enemy || enemy->isFriends(hero.GetColor()))
                    continue;
                if (!hero.GetPath().Calculate(enemy->GetIndex()))
                    continue;
                return true;
            }
        }

        // can pickup objects
        if (conf.ExtHeroPatrolAllowPickup())
        {
            const Maps::Indexes& indicesAround = Maps::ScanAroundObjects(hero.GetIndex(),
                                                                   hero.GetSquarePatrol(), objs1);
            for (auto result : indicesAround)
            {
                if (!HeroesValidObject(hero, result) || !hero.GetPath().Calculate(result))
                    continue;
                ai_objects.erase(result);

                return true;
            }
        }

        // random move
        /*
        // disable move: https://sourceforge.net/tracker/?func=detail&aid=3157397&group_id=96859&atid=616180
        {
            Maps::ScanAroundObject(hero.GetIndex(), hero.GetSquarePatrol(), MP2::OBJ_ZERO);
            if(results.size())
            {
            std::random_shuffle(results.begin(), results.end());
            std::vector<s32>::const_iterator it = results.begin();
            for(; it != results.end(); ++it)
                if(world.GetTiles(*it).isPassable(&hero, Direction::CENTER, true) &&
                hero.GetPath().Calculate(*it))
            {
                return;
            }
            }
        }
        */

        hero.SetModes(HEROES_STUPID);
        return false;
    }

    if (ai_hero.fix_loop > 3)
    {
        hero.SetModes(hero.Modes(HEROES_WAITING) ? HEROES_STUPID : HEROES_WAITING);
        return false;
    }

    // primary target
    if (Maps::isValidAbsIndex(ai_hero.primary_target))
    {
        if (hero.GetIndex() == ai_hero.primary_target)
        {
            ai_hero.primary_target = -1;
            hero.GetPath().Reset();
        }
        else
        {
            const Castle* castle = nullptr;

            if (nullptr != (castle = world.GetCastle(Maps::GetPoint(ai_hero.primary_target))) &&
                nullptr != castle->GetHeroes().Guest() && castle->isFriends(hero.GetColor()))
            {
                hero.SetModes(HEROES_WAITING);
            }

            // make path
            if (ai_hero.primary_target != hero.GetPath().GetDestinationIndex() &&
                !hero.GetPath().Calculate(ai_hero.primary_target))
            {
                ai_hero.primary_target = -1;
            }
        }

        if (hero.GetPath().isValid()) return true;
    }

    // scan heroes and castle
    const Maps::Indexes& enemies = Maps::ScanAroundObjects(hero.GetIndex(), hero.GetScoute(), objs3);

    for (int enemie : enemies)
    {
        if (!AIHeroesPriorityObject(hero, enemie) || !hero.GetPath().Calculate(enemie))
            continue;

        ai_hero.primary_target = enemie;
        return true;
    }

    // check destination
    if (hero.GetPath().isValid())
    {
        if (!HeroesValidObject(hero, hero.GetPath().GetDestinationIndex()))
            hero.GetPath().Reset();
        else if (hero.GetPath().size() < 5)
        {
            ai_hero.fix_loop++;
            return true;
        }
    }

    // scan 2x2 pickup objects
    Maps::Indexes pickups = Maps::ScanAroundObjects(hero.GetIndex(), 2, objs1);
    // scan 3x3 capture objects
    const Maps::Indexes& captures = Maps::ScanAroundObjects(hero.GetIndex(), 3, objs2);
    if (!captures.empty()) pickups.insert(pickups.end(), captures.begin(), captures.end());

    if (!pickups.empty())
    {
        hero.GetPath().Reset();

        for (int& pickup : pickups)
        {
            if (!HeroesValidObject(hero, pickup))
                continue;
            task.push_front(pickup);
        }
    }

    if (hero.GetPath().isValid())
    {
        ai_hero.fix_loop++;
        return true;
    }

    if (task.empty())
    {
        // get task from kingdom
        AIHeroesAddedTask(hero);
    }
    else
    {
		// remove invalid task
		task.remove_if([&](s32&it)
		{
			return !AIHeroesValidObject2(&hero, it);
		});
    }

    // random shuffle
    if (1 < task.size() && Rand::Get(1))
    {
        const auto it1 = task.begin();
        auto it2 = it1;
        ++it2;

        swap(*it1, *it2);
    }

    // find passable index
    while (!task.empty())
    {
        const s32& index = task.front();
        if (hero.GetPath().Calculate(index)) break;

        task.pop_front();
    }

    // success
    if (!task.empty())
    {
        const s32& index = task.front();

        ai_objects.erase(index);
        task.pop_front();

        return true;
    }
    if (hero.Modes(HEROES_WAITING))
    {
        hero.GetPath().Reset();

        hero.ResetModes(HEROES_WAITING);
        hero.SetModes(HEROES_STUPID);
    }
    else
    {
        hero.SetModes(HEROES_WAITING);
    }

    return false;
}

void AIHeroesTurn(Heroes* hero)
{
    if (hero) AI::HeroesTurn(*hero);
}

void AI::HeroesTurn(Heroes& hero)
{
    Interface::StatusWindow& status = Interface::Basic::Get().GetStatusWindow();

    while (hero.MayStillMove() &&
        !hero.Modes(HEROES_WAITING | HEROES_STUPID))
    {
        // turn indicator
        status.RedrawTurnProgress(3);

        // get task for heroes
        HeroesGetTask(hero);

        // turn indicator
        status.RedrawTurnProgress(5);

        // heroes AI turn
        HeroesMove(hero);

        // turn indicator
        status.RedrawTurnProgress(7);
    }
}

bool AIHeroesScheduledVisit(const Kingdom& kingdom, s32 index)
{
    for (auto it : kingdom.GetHeroes()._items)
    {
        AIHero& ai_hero = AIHeroes::Get(*it);
        Queue& task = ai_hero.sheduled_visit;
        if (task.isPresent(index)) return true;
    }
    return false;
}

bool IsPriorityAndNotVisitAndNotPresent(const pair<s32, int>& indexObj, const Heroes* hero)
{
    AIHero& ai_hero = AIHeroes::Get(*hero);
    Queue& task = ai_hero.sheduled_visit;

    return AIHeroesPriorityObject(*hero, indexObj.first) &&
        !AIHeroesScheduledVisit(hero->GetKingdom(), indexObj.first) &&
        !task.isPresent(indexObj.first);
}

void AIHeroesEnd(Heroes* hero)
{
    if (!hero)
        return;
    AIHero& ai_hero = AIHeroes::Get(*hero);
    AIKingdom& ai_kingdom = AIKingdoms::Get(hero->GetColor());
    Queue& task = ai_hero.sheduled_visit;
    IndexObjectMap& ai_objects = ai_kingdom.scans;

    if (hero->Modes(AI::HEROES_WAITING | AI::HEROES_STUPID))
    {
        ai_hero.Reset();
        hero->ResetModes(AI::HEROES_WAITING | AI::HEROES_STUPID);
    }

    IndexObjectMap::iterator it;

    while (true)
    {
        for (it = ai_objects.begin(); it != ai_objects.end(); ++it)
            if (IsPriorityAndNotVisitAndNotPresent(*it, hero)) break;

        if (ai_objects.end() == it) break;
        task.push_front((*it).first);
        ai_objects.erase((*it).first);
    }
}


void AIHeroesSetHunterWithTarget(Heroes* hero, s32 dst)
{
    if (!hero)
        return;
    AIHero& ai_hero = AIHeroes::Get(*hero);

    hero->SetModes(AI::HEROES_HUNTER);

    if (0 > ai_hero.primary_target)
    {
        ai_hero.primary_target = dst;
    }
}

void AIHeroesCaptureNearestTown(Heroes* hero)
{
    if (!hero) return;
    AIHero& ai_hero = AIHeroes::Get(*hero);

    if (0 <= ai_hero.primary_target)
        return;
    const Maps::Indexes& castles = Maps::GetObjectPositions(hero->GetIndex(), MP2::OBJ_CASTLE, true);

    for (int it : castles)
    {
        const Castle* castle = world.GetCastle(Maps::GetPoint(it));

        if (castle &&
            Army::TroopsStrongerEnemyTroops(hero->GetArmy().m_troops, castle->GetArmy().m_troops))
        {
            ai_hero.primary_target = it;

            break;
        }
    }
}
