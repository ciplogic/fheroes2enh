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
#include "game_interface.h"
#include "heroes.h"
#include "castle.h"
#include "world.h"
#include "agg.h"
#include "ai.h"
#include "ai_simple.h"
#include "mus.h"

void AICastleTurn(Castle *);

void AIHeroesTurn(Heroes *);

void AIHeroesEnd(Heroes *);

void AIHeroesCaptureNearestTown(Heroes *);

void AIHeroesSetHunterWithTarget(Heroes *, s32);

AIKingdoms &AIKingdoms::Get()
{
    static AIKingdoms ai_kingdoms;
    return ai_kingdoms;
}

AIKingdom &AIKingdoms::Get(int color)
{
    return Get()._items.at(Color::GetIndex(color));
}

void AIKingdoms::Reset()
{
    AIKingdoms &ai = Get();
    for_each(ai._items.begin(), ai._items.end(), mem_fun_ref(&AIKingdom::Reset));
}

void AIKingdom::Reset()
{
    capital = nullptr;
    scans.clear();
}

void IndexObjectMap::DumpObjects(const IndexDistance &id)
{
}

void WorldStoreObjects(int color, IndexObjectMap &store)
{
    for (s32 it = 0; it < world.w() * world.h(); ++it)
    {
        const Maps::Tiles &tile = world.GetTiles(it);
        if (tile.isFog(color)) continue;

        if (MP2::isGroundObject(tile.GetObject()) ||
            MP2::isWaterObject(tile.GetObject()) || MP2::OBJ_HEROES == tile.GetObject())
        {
            // if quantity object is empty
            if (MP2::isQuantityObject(tile.GetObject()) &&
                !MP2::isPickupObject(tile.GetObject()) && !tile.QuantityIsValid())
                continue;

            // skip captured obj
            if (MP2::isCaptureObject(tile.GetObject()) &&
                Players::isFriends(color, tile.QuantityColor()))
                continue;

            // skip for meeting heroes
            if (MP2::OBJ_HEROES == tile.GetObject())
            {
                const Heroes *hero = tile.GetHeroes();
                if (hero && color == hero->GetColor()) continue;
            }

            // check: is visited objects
            switch (tile.GetObject())
            {
                case MP2::OBJ_MAGELLANMAPS:
                case MP2::OBJ_OBSERVATIONTOWER:
                    if (world.GetKingdom(color).isVisited(tile)) continue;
                    break;

                default:
                    break;
            }

            store[it] = tile.GetObject();
        }
    }
}

void AI::KingdomTurn(Kingdom &kingdom)
{
    KingdomHeroes &heroes = kingdom.GetHeroes();
    KingdomCastles &castles = kingdom.GetCastles();

    const int color = kingdom.GetColor();

    if (kingdom.isLoss() || color == Color::NONE)
    {
        kingdom.LossPostActions();
        return;
    }

    if (!Settings::Get().MusicMIDI()) AGG::PlayMusic(MUS::COMPUTER);

    Interface::StatusWindow &status = Interface::Basic::Get().GetStatusWindow();
    AIKingdom &ai = AIKingdoms::Get(color);

    // turn indicator
    status.RedrawTurnProgress(0);

    // scan map
    ai.scans.clear();
    WorldStoreObjects(color, ai.scans);

    // set capital
    if (nullptr == ai.capital && !castles.empty())
    {
        auto it = find_if(castles.begin(), castles.end(), Castle::PredicateIsCastle);

        if (castles.end() != it)
        {
            if (*it)
            {
                ai.capital = *it;
                ai.capital->SetModes(Castle::CAPITAL);
            }
        } else
            // first town
        {
            ai.capital = castles.front();
            ai.capital->SetModes(Castle::CAPITAL);
        }
    }

    // turn indicator
    status.RedrawTurnProgress(1);

    // castles AI turn
    for_each(castles.begin(), castles.end(), [](auto *castle) { AICastleTurn(castle); });

    // need capture town?
    if (castles.empty())
        for_each(heroes._items.begin(), heroes._items.end(), [](auto *castle) {
            AIHeroesCaptureNearestTown(castle);
        });

    // buy hero in capital
    if (ai.capital && ai.capital->isCastle())
    {
        uint32_t modes = 0;
        const uint32_t maxhero = (s32) mapsize_t::XLARGE > world.w() ? ((s32) mapsize_t::LARGE > world.w() ? 3 : 2) : 4;

        if (heroes._items.empty())
            modes = HEROES_HUNTER | HEROES_SCOUTER;
        else if (heroes._items.size() < maxhero ||
                 0 == count_if(heroes._items.begin(), heroes._items.end(),
                               bind2nd(mem_fun(&Heroes::Modes), HEROES_SCOUTER)))
            modes = HEROES_SCOUTER;

        if (modes &&
            heroes._items.size() < Kingdom::GetMaxHeroes())
        {
            Recruits &rec = kingdom.GetRecruits();
            Heroes *hero = ai.capital->GetHeroes().Guest();

            if (!hero)
            {
                if (rec.GetHero1() && rec.GetHero2())
                    hero = ai.capital->RecruitHero(
                            rec.GetHero1()->GetLevel() >= rec.GetHero2()->GetLevel() ? rec.GetHero1() : rec.GetHero2());
                else if (rec.GetHero1())
                    hero = ai.capital->RecruitHero(rec.GetHero1());
                else if (rec.GetHero2())
                    hero = ai.capital->RecruitHero(rec.GetHero2());

                if (hero)
                    hero->SetModes(modes);
            }
        }
    }

    // set hunters
    if (ai.capital)
    {
        const size_t hunters =
                count_if(heroes._items.begin(), heroes._items.end(),
                    [&](const Heroes* hero)
        {
            return hero->Modes(HEROES_HUNTER);
        });

        // every time
        if (0 == hunters && !heroes._items.empty())
        {
            auto it = find_if(heroes._items.begin(), heroes._items.end(),
                              not1(bind2nd(mem_fun(&Heroes::Modes),
                                           Heroes::PATROL)));

            if (it != heroes._items.end() &&
                !ai.capital->GetHeroes().Guest())
                AIHeroesSetHunterWithTarget(*it, ai.capital->GetIndex());
        } else
            // each month
        if (world.BeginMonth() && 1 < world.CountDay())
        {
            auto it = find_if(heroes._items.begin(), heroes._items.end(),
                              bind2nd(mem_fun(&Heroes::Modes), HEROES_HUNTER));

            if (it != heroes._items.end() &&
                !ai.capital->GetHeroes().Guest())
                AIHeroesSetHunterWithTarget(*it, ai.capital->GetIndex());
        }
    }

    // update roles
    {
        for_each(heroes._items.begin(), heroes._items.end(),
                 bind2nd(mem_fun(&Heroes::ResetModes), HEROES_STUPID | HEROES_WAITING));

        // init roles
        if (heroes._items.end() != find_if(heroes._items.begin(), heroes._items.end(),
                                    not1(bind2nd(mem_fun(&Heroes::Modes),
                                                 HEROES_SCOUTER | HEROES_HUNTER))))
        {
            vector<Heroes *>::iterator ith, first = heroes._items.end();

            while (heroes._items.end() != (ith = find_if(heroes._items.begin(), heroes._items.end(),
                                                  not1(bind2nd(mem_fun(&Heroes::Modes),
                                                          // also skip patrol
                                                               HEROES_HUNTER | HEROES_SCOUTER |
                                                               Heroes::PATROL)))))
            {
                if (first == heroes._items.end())
                {
                    first = ith;
                    if (*ith) (*ith)->SetModes(HEROES_HUNTER | HEROES_SCOUTER);
                } else if (*ith) (*ith)->SetModes(HEROES_SCOUTER);
            }
        }
    }

    // turn indicator
    status.RedrawTurnProgress(2);

    // heroes turns
    for_each(heroes._items.begin(), heroes._items.end(), [](Heroes *hero) { AIHeroesTurn(hero); });
    //std::for_each(heroes.begin(), heroes.end(), std::bind2nd(std::mem_fun(&Heroes::ResetModes), AI::HEROES_STUPID|AI::HEROES_WAITING));
    for_each(heroes._items.begin(), heroes._items.end(), [](Heroes *hero) { AIHeroesTurn(hero); });
    for_each(heroes._items.begin(), heroes._items.end(), [](Heroes *hero) { AIHeroesEnd(hero); });

    // turn indicator
    status.RedrawTurnProgress(9);
}
