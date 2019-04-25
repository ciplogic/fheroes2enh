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

#include <algorithm>

#include "settings.h"
#include "castle.h"
#include "game.h"
#include "profit.h"
#include "world.h"
#include "race.h"
#include "battle.h"
#include "game_static.h"
#include "game_interface.h"
#include "ai.h"
#include "bitmodes.h"
#include <functional>

bool HeroesStrongestArmy(const Heroes* h1, const Heroes* h2)
{
    return h1 && h2 && Army::TroopsStrongerEnemyTroops(h2->GetArmy().m_troops, h1->GetArmy().m_troops);
}

Kingdom::Kingdom() : color(Color::NONE), lost_town_days(0), visited_tents_colors(0)
{
    heroes_cond_loss._items.reserve(4);
}

void Kingdom::Init(int clr)
{
    clear();
    color = clr;

    if (Color::ALL & color)
    {
        heroes._items.reserve(GetMaxHeroes());
        castles._items.reserve(15);

        UpdateStartingResource();
    }
}

void Kingdom::clear()
{
    modes = 0;

    color = Color::NONE;
    visited_tents_colors = 0;
    lost_town_days = Game::GetLostTownDays() + 1;

    heroes._items.clear();
    castles._items.clear();
    visit_object.clear();

    recruits.Reset();

    heroes_cond_loss._items.clear();
}

int Kingdom::GetControl() const
{
    return Players::GetPlayerControl(color);
}

int Kingdom::GetColor() const
{
    return color;
}

int Kingdom::GetRace() const
{
    return Players::GetPlayerRace(GetColor());
}

void Kingdom::UpdateStartingResource()
{
    resource = GameStatic::GetKingdomStartingResource(isControlAI() ? 5 : Settings::Get().GameDifficulty());
}

bool Kingdom::isLoss() const
{
    return castles._items.empty() && heroes._items.empty();
}

bool Kingdom::isPlay() const
{
    return Players::GetPlayerInGame(color);
}

void Kingdom::LossPostActions()
{
    if (!isPlay())
        return;
    Players::SetPlayerInGame(color, false);

    if (!heroes._items.empty())
    {
        for_each(heroes._items.begin(), heroes._items.end(),
                 [](Heroes* hero) { hero->SetFreeman(static_cast<int>(Battle::RESULT_LOSS)); });
        heroes._items.clear();
    }
    if (!castles._items.empty())
    {
        castles.ChangeColors(GetColor(), Color::NONE);
        castles._items.clear();
    }
    world.ResetCapturedObjects(GetColor());
}

void Kingdom::ActionBeforeTurn()
{
    // rescan heroes path
    for_each(heroes._items.begin(), heroes._items.end(),
             [](Heroes* it) { it->RescanPath(); });
}

void Kingdom::ActionNewDay()
{
    if (isLoss() || 0 == lost_town_days)
    {
        LossPostActions();
        return;
    }

    // modes
    ResetModes(IDENTIFYHERO);

    // check lost town
    if (castles._items.empty()) --lost_town_days;

    // skip incomes for first day
    if (1 < world.CountDay())
    {
        // castle New Day
        for_each(castles._items.begin(), castles._items.end(),
                 [](Castle* it) { it->ActionNewDay(); });

        // heroes New Day
        for_each(heroes._items.begin(), heroes._items.end(),
                 [](Heroes* hero) { hero->ActionNewDay(); });

        // income
        AddFundsResource(GetIncome());
    }

    // check event day AI
    EventsDate events = world.GetEventsDate(GetColor());
    for (const auto& event : events)
        AddFundsResource(event.resource);

    // remove day visit object
    visit_object.remove_if(Visit::isDayLife);
}

void Kingdom::ActionNewWeek()
{
    ResetModes(DISABLEHIRES);

    // skip first day
    if (1 < world.CountDay())
    {
        // castle New Week
        for_each(castles._items.begin(), castles._items.end(),
                 [](Castle* it) { it->ActionNewWeek(); });

        // heroes New Week
        for_each(heroes._items.begin(), heroes._items.end(),
                 [](Heroes* hero) { hero->ActionNewWeek(); });


        // debug an gift
        if (IS_DEVEL() && isControlHuman())
        {
            Funds gift(20, 20, 10, 10, 10, 10, 5000);
            resource += gift;
        }
    }

    // remove week visit object
    visit_object.remove_if(Visit::isWeekLife);

    UpdateRecruits();
}

void Kingdom::ActionNewMonth()
{
    // skip first day
    if (1 < world.CountDay())
    {
        // castle New Month
        for_each(castles._items.begin(), castles._items.end(),
                 [](Castle* it) { it->ActionNewMonth(); });

        // heroes New Month
        for_each(heroes._items.begin(), heroes._items.end(),
                 [](Heroes* hero) { hero->ActionNewMonth(); });
    }

    // remove week visit object
    visit_object.remove_if(Visit::isMonthLife);
}

void Kingdom::AddHeroes(Heroes* hero)
{
    if (!hero)
        return;
    if (heroes._items.end() == find(heroes._items.begin(), heroes._items.end(), hero))
        heroes._items.push_back(hero);

    auto player = Settings::Get().GetPlayers().GetCurrent();
    if (player && player->isColor(GetColor()))
        Interface::Basic::Get().GetIconsPanel().ResetIcons(icons_t::ICON_HEROES);

    AI::HeroesAdd(*hero);
}

void Kingdom::AddHeroStartCondLoss(Heroes* hero)
{
    // see: Settings::ExtWorldStartHeroLossCond4Humans
    heroes_cond_loss._items.push_back(hero);
}

const Heroes* Kingdom::GetFirstHeroStartCondLoss() const
{
    for (auto heroes_cond_los : heroes_cond_loss._items)
        if (heroes_cond_los->isFreeman() || heroes_cond_los->GetColor() != GetColor())
            return heroes_cond_los;
    return nullptr;
}

string Kingdom::GetNamesHeroStartCondLoss() const
{
    string result;
    for (auto it = heroes_cond_loss._items.begin(); it != heroes_cond_loss._items.end(); ++it)
    {
        result.append((*it)->GetName());
        if (it + 1 != heroes_cond_loss._items.end()) result.append(", ");
    }
    return result;
}

void Kingdom::RemoveHeroes(const Heroes* hero)
{
    if (hero)
    {
        if (!heroes._items.empty())
            heroes._items.erase(find(heroes._items.begin(), heroes._items.end(), hero));

        AI::HeroesRemove(*hero);
    }

    if (isLoss()) LossPostActions();
}

void Kingdom::AddCastle(const Castle* castle)
{
    if (castle)
    {
        if (castles._items.end() == find(castles._items.begin(), castles._items.end(), castle))
            castles._items.push_back(const_cast<Castle *>(castle));

        auto player = Settings::Get().GetPlayers().GetCurrent();
        if (player && player->isColor(GetColor()))
            Interface::Basic::Get().GetIconsPanel().ResetIcons(icons_t::ICON_CASTLES);

        AI::CastleAdd(*castle);
    }

    lost_town_days = Game::GetLostTownDays() + 1;
}

void Kingdom::RemoveCastle(const Castle* castle)
{
    if (castle)
    {
        if (!castles._items.empty())
            castles._items.erase(find(castles._items.begin(), castles._items.end(), castle));

        AI::CastleRemove(*castle);
    }

    if (isLoss()) LossPostActions();
}

uint32_t Kingdom::GetCountCastle() const
{
    return std::count_if(castles._items.begin(), castles._items.end(), Castle::PredicateIsCastle);
}

uint32_t Kingdom::GetCountTown() const
{
    return std::count_if(castles._items.begin(), castles._items.end(), Castle::PredicateIsTown);
}

uint32_t Kingdom::GetCountMarketplace() const
{
    return std::count_if(castles._items.begin(), castles._items.end(), Castle::PredicateIsBuildMarketplace);
}

uint32_t Kingdom::GetCountNecromancyShrineBuild() const
{
    return std::count_if(castles._items.begin(), castles._items.end(),
                         [](Castle* it) { return it->isNecromancyShrineBuild(); });
}

uint32_t Kingdom::GetCountBuilding(uint32_t build) const
{
    return std::count_if(castles._items.begin(), castles._items.end(),
                         [&](const Castle* it) { return it->isBuild(build); });
}

bool Kingdom::AllowPayment(const Funds& funds) const
{
    return
        (resource.wood >= funds.wood || 0 == funds.wood) &&
        (resource.mercury >= funds.mercury || 0 == funds.mercury) &&
        (resource.ore >= funds.ore || 0 == funds.ore) &&
        (resource.sulfur >= funds.sulfur || 0 == funds.sulfur) &&
        (resource.crystal >= funds.crystal || 0 == funds.crystal) &&
        (resource.gems >= funds.gems || 0 == funds.gems) &&
        (resource.gold >= funds.gold || 0 == funds.gold);
}

/* is visited cell */
bool Kingdom::isVisited(const Maps::Tiles& tile) const
{
    return isVisited(tile.GetIndex(), tile.GetObject());
}

bool Kingdom::isVisited(s32 index, int object) const
{
    const auto it = find_if(visit_object.begin(), visit_object.end(),
                            [&](const IndexObject& it)
                            {
                                return it.isIndex(index);
                            });

    return visit_object.end() != it && (*it).isObject(object);
}

/* return true if object visited */
bool Kingdom::isVisited(int object) const
{
    return visit_object.end() != find_if(visit_object.begin(), visit_object.end(),
                                         [&](const IndexObject& it) { return it.isObject(object); });
}

uint32_t Kingdom::CountVisitedObjects(int object) const
{
    return count_if(visit_object.begin(), visit_object.end(),
                    [&](const IndexObject& it) { return it.isObject(object); });
}

/* set visited cell */
void Kingdom::SetVisited(s32 index, int object)
{
    if (!isVisited(index, object) && object != MP2::OBJ_ZERO) visit_object.push_front(IndexObject(index, object));
}

bool Kingdom::HeroesMayStillMove() const
{
    return heroes._items.end() != find_if(heroes._items.begin(), heroes._items.end(),
                                          [](const Heroes* hero) { return hero->MayStillMove(); });
}

uint32_t Kingdom::GetCountCapital() const
{
    return count_if(castles._items.begin(), castles._items.end(), Castle::PredicateIsCapital);
}

void Kingdom::AddFundsResource(const Funds& funds)
{
    resource = resource + funds;
}

void Kingdom::OddFundsResource(const Funds& funds)
{
    resource = resource - funds;
}

uint32_t Kingdom::GetLostTownDays() const
{
    return lost_town_days;
}

Recruits& Kingdom::GetRecruits()
{
    // update hero1
    if (Heroes::UNKNOWN == recruits.GetID1() || (recruits.GetHero1() && !recruits.GetHero1()->isFreeman()))
        recruits.SetHero1(world.GetFreemanHeroes(GetRace()));

    // update hero2
    if (Heroes::UNKNOWN == recruits.GetID2() || (recruits.GetHero2() && !recruits.GetHero2()->isFreeman()))
        recruits.SetHero2(world.GetFreemanHeroes());

    if (recruits.GetID1() == recruits.GetID2()) world.UpdateRecruits(recruits);

    return recruits;
}

void Kingdom::UpdateRecruits()
{
    recruits.SetHero1(world.GetFreemanHeroes(GetRace()));
    recruits.SetHero2(world.GetFreemanHeroes());

    if (recruits.GetID1() == recruits.GetID2()) world.UpdateRecruits(recruits);
}

const Puzzle& Kingdom::PuzzleMaps() const
{
    return puzzle_maps;
}

Puzzle& Kingdom::PuzzleMaps()
{
    return puzzle_maps;
}

void Kingdom::SetVisitTravelersTent(int col)
{
    visited_tents_colors |= col;
}

bool Kingdom::IsVisitTravelersTent(int col) const
{
    return visited_tents_colors & col;
}

bool Kingdom::AllowRecruitHero(bool check_payment, int level) const
{
    return heroes._items.size() < GetMaxHeroes() && (!check_payment || AllowPayment(
        PaymentConditions::RecruitHero(level)));
}

void Kingdom::ApplyPlayWithStartingHero()
{
    if (isPlay() && !castles._items.empty())
    {
        // get first castle
        Castle* first = castles.GetFirstCastle();
        if (nullptr == first) first = castles._items.front();

        // check manual set hero (castle position + point(0, 1))?
        const Point& cp = first->GetCenter();
        Heroes* hero = world.GetTiles(cp.x, cp.y + 1).GetHeroes();

        // and move manual set hero to castle
        if (hero && hero->GetColor() == GetColor())
        {
            const bool patrol = hero->Modes(Heroes::PATROL);
            hero->SetFreeman(0);
            hero->Recruit(*first);

            if (patrol)
            {
                hero->SetModes(Heroes::PATROL);
                hero->SetCenterPatrol(cp);
            }
        }
        else if (Settings::Get().GameStartWithHeroes())
        {
            Heroes* hero = world.GetFreemanHeroes(first->GetRace());
            if (hero && AllowRecruitHero(false, 0)) hero->Recruit(*first);
        }
    }
}

uint32_t Kingdom::GetMaxHeroes()
{
    return GameStatic::GetKingdomMaxHeroes();
}

void Kingdom::HeroesActionNewPosition() const
{
    // Heroes::ActionNewPosition: can remove elements from heroes vector.
    KingdomHeroes heroes2(heroes);
    for_each(heroes2._items.begin(), heroes2._items.end(),
             [](Heroes* it) { it->ActionNewPosition(); });
}

Funds Kingdom::GetIncome(int type /* INCOME_ALL */) const
{
    Funds resource;

    if (INCOME_CAPTURED & type)
    {
        // captured object
        const int resources[] = {
            Resource::WOOD, Resource::ORE, Resource::MERCURY, Resource::SULFUR,
            Resource::CRYSTAL, Resource::GEMS, Resource::GOLD, Resource::UNKNOWN
        };

        for (uint32_t index = 0; resources[index] != Resource::UNKNOWN; ++index)
            resource += ProfitConditions::FromMine(resources[index]) *
                world.CountCapturedMines(resources[index], GetColor());
    }

    if (INCOME_CASTLES & type)
    {
        // castles
        for (auto it : castles._items)
        {
            const Castle& castle = *it;

            // castle or town profit
            resource += ProfitConditions::FromBuilding(castle.isCastle() ? BUILD_CASTLE : BUILD_TENT, 0);

            // statue
            if (castle.isBuild(BUILD_STATUE))
                resource += ProfitConditions::FromBuilding(BUILD_STATUE, 0);

            // dungeon for warlock
            if (castle.isBuild(BUILD_SPEC) && Race::WRLK == castle.GetRace())
                resource += ProfitConditions::FromBuilding(BUILD_SPEC, Race::WRLK);
        }
    }

    if (INCOME_ARTIFACTS & type)
    {
        // find artifacts
        const int artifacts[] = {
            Artifact::GOLDEN_GOOSE, Artifact::ENDLESS_SACK_GOLD, Artifact::ENDLESS_BAG_GOLD,
            Artifact::ENDLESS_PURSE_GOLD, Artifact::ENDLESS_POUCH_SULFUR,
            Artifact::ENDLESS_VIAL_MERCURY,
            Artifact::ENDLESS_POUCH_GEMS, Artifact::ENDLESS_CORD_WOOD, Artifact::ENDLESS_CART_ORE,
            Artifact::ENDLESS_POUCH_CRYSTAL, Artifact::UNKNOWN
        };

        for (uint32_t index = 0; artifacts[index] != Artifact::UNKNOWN; ++index)
            for (auto heroe : heroes._items)
                resource += ProfitConditions::FromArtifact(artifacts[index]) *
                    (*heroe).GetBagArtifacts().Count(Artifact(artifacts[index]));

        // TAX_LIEN
        for (auto heroe : heroes._items)
            resource -= ProfitConditions::FromArtifact(Artifact::TAX_LIEN) *
                (*heroe).GetBagArtifacts().Count(Artifact(Artifact::TAX_LIEN));
    }

    if (INCOME_HEROSKILLS & type)
    {
        // estates skill bonus
        for (auto heroe : heroes._items)
            resource.gold += (*heroe).GetSecondaryValues(Skill::SkillT::ESTATES);
    }

    return resource;
}

const Heroes* Kingdom::GetBestHero() const
{
    return !heroes._items.empty()
               ? *max_element(heroes._items.begin(), heroes._items.end(), HeroesStrongestArmy)
               : nullptr;
}

uint32_t Kingdom::GetArmiesStrength() const
{
    uint32_t res = 0;

    for (auto heroe : heroes._items)
        res += (*heroe).GetArmy().m_troops.GetStrength();

    for (auto castle : castles._items)
        res += (*castle).GetArmy().m_troops.GetStrength();

    return res;
}

Kingdoms::Kingdoms()
= default;

void Kingdoms::Init()
{
    const Colors colors(Settings::Get().GetPlayers().GetColors());

    clear();

    for (int color : colors._items)
        GetKingdom(color).Init(color);
}

uint32_t Kingdoms::size()
{
    return KINGDOMMAX + 1;
}

void Kingdoms::clear()
{
    for (uint32_t ii = 0; ii < size(); ++ii)
        kingdoms[ii].clear();
}

void Kingdoms::ApplyPlayWithStartingHero()
{
    for (uint32_t ii = 0; ii < size(); ++ii)
        if (kingdoms[ii].isPlay()) kingdoms[ii].ApplyPlayWithStartingHero();
}

const Kingdom& Kingdoms::GetKingdom(int color) const
{
    switch (color)
    {
    case Color::BLUE:
        return kingdoms[0];
    case Color::GREEN:
        return kingdoms[1];
    case Color::RED:
        return kingdoms[2];
    case Color::YELLOW:
        return kingdoms[3];
    case Color::ORANGE:
        return kingdoms[4];
    case Color::PURPLE:
        return kingdoms[5];
    default:
        break;
    }

    return kingdoms[6];
}

Kingdom& Kingdoms::GetKingdom(int color)
{
    switch (color)
    {
    case Color::BLUE:
        return kingdoms[0];
    case Color::GREEN:
        return kingdoms[1];
    case Color::RED:
        return kingdoms[2];
    case Color::YELLOW:
        return kingdoms[3];
    case Color::ORANGE:
        return kingdoms[4];
    case Color::PURPLE:
        return kingdoms[5];
    default:
        break;
    }

    return kingdoms[6];
}

void Kingdom::SetLastLostHero(Heroes& hero)
{
    lost_hero.first = hero.GetID();
    lost_hero.second = world.CountDay();
}

void Kingdom::ResetLastLostHero()
{
    lost_hero.first = Heroes::UNKNOWN;
    lost_hero.second = 0;
}

Heroes* Kingdom::GetLastLostHero() const
{
    return Heroes::UNKNOWN != lost_hero.first && world.CountDay() - lost_hero.second < DAYOFWEEK
               ? world.GetHeroes(lost_hero.first)
               : nullptr;
}

void Kingdoms::NewDay()
{
    for (uint32_t ii = 0; ii < size(); ++ii)
        if (kingdoms[ii].isPlay()) kingdoms[ii].ActionNewDay();
}

void Kingdoms::NewWeek()
{
    for (uint32_t ii = 0; ii < size(); ++ii)
        if (kingdoms[ii].isPlay()) kingdoms[ii].ActionNewWeek();
}

void Kingdoms::NewMonth()
{
    for (uint32_t ii = 0; ii < size(); ++ii)
        if (kingdoms[ii].isPlay()) kingdoms[ii].ActionNewMonth();
}

int Kingdoms::GetNotLossColors() const
{
    int result = 0;
    for (uint32_t ii = 0; ii < size(); ++ii)
        if (kingdoms[ii].GetColor() &&
            !kingdoms[ii].isLoss())
            result |= kingdoms[ii].GetColor();
    return result;
}

int Kingdoms::GetLossColors() const
{
    int result = 0;
    for (uint32_t ii = 0; ii < size(); ++ii)
        if (kingdoms[ii].GetColor() &&
            kingdoms[ii].isLoss())
            result |= kingdoms[ii].GetColor();
    return result;
}

int Kingdoms::FindWins(int cond) const
{
    for (uint32_t ii = 0; ii < size(); ++ii)
        if (kingdoms[ii].GetColor() &&
            world.KingdomIsWins(kingdoms[ii], cond))
            return kingdoms[ii].GetColor();
    return 0;
}

void Kingdoms::AddHeroes(const AllHeroes& heroes)
{
    for (auto heroe : heroes._items)
        // skip gray color
        if (heroe->GetColor()) GetKingdom(heroe->GetColor()).AddHeroes(heroe);
}

void Kingdoms::AddCondLossHeroes(const AllHeroes& heroes)
{
    for (auto heroe : heroes._items)
        // skip gray color
        if (heroe->GetColor())
        {
            Kingdom& kingdom = GetKingdom(heroe->GetColor());

            if (kingdom.isControlHuman())
            {
                heroe->SetModes(Heroes::NOTDISMISS | Heroes::NOTDEFAULTS);
                kingdom.AddHeroStartCondLoss(heroe);
            }
        }
}

void Kingdoms::AddCastles(const AllCastles& castles)
{
    for (auto castle : castles._items)
        // skip gray color
        if (castle->GetColor()) GetKingdom(castle->GetColor()).AddCastle(castle);
}

void Kingdoms::AddTributeEvents(CapturedObjects& captureobj, uint32_t day, int obj)
{
    for (uint32_t ii = 0; ii < size(); ++ii)
        if (kingdoms[ii].isPlay())
        {
            const int color = kingdoms[ii].GetColor();
            const Funds& funds = captureobj.TributeCapturedObject(color, obj);

            kingdoms[ii].AddFundsResource(funds);

            // for show dialogs
            if (funds.GetValidItemsCount() && kingdoms[ii].isControlHuman())
            {
                EventDate event;

                event.computer = true;
                event.first = day;
                event.colors = color;
                event.resource = funds;
                event.message = MP2::StringObject(obj);

                world.AddEventDate(event);
            }
        }
}

ByteVectorWriter& operator<<(ByteVectorWriter& msg, const Kingdom& kingdom)
{
    return msg <<
        kingdom.modes <<
        kingdom.color <<
        kingdom.resource <<
        kingdom.lost_town_days <<
        kingdom.castles <<
        kingdom.heroes <<
        kingdom.recruits <<
        kingdom.lost_hero <<
        kingdom.visit_object <<
        kingdom.puzzle_maps <<
        kingdom.visited_tents_colors <<
        kingdom.heroes_cond_loss;
}

ByteVectorReader& operator>>(ByteVectorReader& msg, Kingdom& kingdom)
{
    return msg >>
        kingdom.modes >>
        kingdom.color >>
        kingdom.resource >>
        kingdom.lost_town_days >>
        kingdom.castles >>
        kingdom.heroes >>
        kingdom.recruits >>
        kingdom.lost_hero >>
        kingdom.visit_object >>
        kingdom.puzzle_maps >>
        kingdom.visited_tents_colors >>
        kingdom.heroes_cond_loss;
}


ByteVectorWriter& operator<<(ByteVectorWriter& msg, const Kingdoms& obj)
{
    msg << static_cast<uint32_t>(obj.size());
    for (uint32_t ii = 0; ii < obj.size(); ++ii)
        msg << obj.kingdoms[ii];

    return msg;
}

ByteVectorReader& operator>>(ByteVectorReader& msg, Kingdoms& obj)
{
    uint32_t kingdomscount;
    msg >> kingdomscount; // FIXME: check kingdomscount

    for (uint32_t ii = 0; ii < kingdomscount; ++ii)
        msg >> obj.kingdoms[ii];

    return msg;
}

ByteVectorReader& operator>>(ByteVectorReader& sb, LastLoseHero& st)
{
    return sb >> st.first >> st.second;
}

KingdomCastles& KingdomCastles::GetNull()
{
    static KingdomCastles nullInstance;
    return nullInstance;
}

KingdomHeroes& KingdomHeroes::GetNull()
{
    static KingdomHeroes nullInstance;
    return nullInstance;
}
