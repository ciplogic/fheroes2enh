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

bool HeroesStrongestArmy(const Heroes *h1, const Heroes *h2)
{
    return h1 && h2 && Army::TroopsStrongerEnemyTroops(h2->GetArmy().m_troops, h1->GetArmy().m_troops);
}

Kingdom::Kingdom() : color(Color::NONE), lost_town_days(0), visited_tents_colors(0)
{
    heroes_cond_loss.reserve(4);
}

void Kingdom::Init(int clr)
{
    clear();
    color = clr;

    if (Color::ALL & color)
    {
        heroes.reserve(GetMaxHeroes());
        castles.reserve(15);

        UpdateStartingResource();
    }
}

void Kingdom::clear()
{
    modes = 0;

    color = Color::NONE;
    visited_tents_colors = 0;
    lost_town_days = Game::GetLostTownDays() + 1;

    heroes.clear();
    castles.clear();
    visit_object.clear();

    recruits.Reset();

    heroes_cond_loss.clear();
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
    return castles.empty() && heroes.empty();
}

bool Kingdom::isPlay() const
{
    return Players::GetPlayerInGame(color);
}

void Kingdom::LossPostActions()
{
    if (isPlay())
    {
        Players::SetPlayerInGame(color, false);

        if (!heroes.empty())
        {
            for_each(heroes.begin(), heroes.end(),
                     bind2nd(mem_fun(&Heroes::SetFreeman), static_cast<int>(Battle::RESULT_LOSS)));
            heroes.clear();
        }
        if (!castles.empty())
        {
            castles.ChangeColors(GetColor(), Color::NONE);
            castles.clear();
        }
        world.ResetCapturedObjects(GetColor());
    }
}

void Kingdom::ActionBeforeTurn()
{
    // rescan heroes path
    for_each(heroes.begin(), heroes.end(), mem_fun(&Heroes::RescanPath));
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
    if (castles.empty()) --lost_town_days;

    // skip incomes for first day
    if (1 < world.CountDay())
    {
        // castle New Day
        for_each(castles.begin(), castles.end(), mem_fun(&Castle::ActionNewDay));

        // heroes New Day
        for_each(heroes.begin(), heroes.end(), mem_fun(&Heroes::ActionNewDay));

        // income
        AddFundsResource(GetIncome());
    }

    // check event day AI
    EventsDate events = world.GetEventsDate(GetColor());
    for (EventsDate::const_iterator
                 it = events.begin(); it != events.end(); ++it)
        AddFundsResource((*it).resource);

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
        for_each(castles.begin(), castles.end(), mem_fun(&Castle::ActionNewWeek));

        // heroes New Week
        for_each(heroes.begin(), heroes.end(), mem_fun(&Heroes::ActionNewWeek));

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
        for_each(castles.begin(), castles.end(), mem_fun(&Castle::ActionNewMonth));

        // heroes New Month
        for_each(heroes.begin(), heroes.end(), mem_fun(&Heroes::ActionNewMonth));
    }

    // remove week visit object
    visit_object.remove_if(Visit::isMonthLife);
}

void Kingdom::AddHeroes(Heroes *hero)
{
    if (hero)
    {
        if (heroes.end() == find(heroes.begin(), heroes.end(), hero))
            heroes.push_back(hero);

        Player *player = Settings::Get().GetPlayers().GetCurrent();
        if (player && player->isColor(GetColor()))
            Interface::Basic::Get().GetIconsPanel().ResetIcons(ICON_HEROES);

        AI::HeroesAdd(*hero);
    }
}

void Kingdom::AddHeroStartCondLoss(Heroes *hero)
{
    // see: Settings::ExtWorldStartHeroLossCond4Humans
    heroes_cond_loss.push_back(hero);
}

const Heroes *Kingdom::GetFirstHeroStartCondLoss() const
{
    for (auto heroes_cond_los : heroes_cond_loss)
        if (heroes_cond_los->isFreeman() || heroes_cond_los->GetColor() != GetColor())
            return heroes_cond_los;
    return nullptr;
}

string Kingdom::GetNamesHeroStartCondLoss() const
{
    string result;
    for (auto
                 it = heroes_cond_loss.begin(); it != heroes_cond_loss.end(); ++it)
    {
        result.append((*it)->GetName());
        if (it + 1 != heroes_cond_loss.end()) result.append(", ");
    }
    return result;
}

void Kingdom::RemoveHeroes(const Heroes *hero)
{
    if (hero)
    {
        if (!heroes.empty())
            heroes.erase(find(heroes.begin(), heroes.end(), hero));

        AI::HeroesRemove(*hero);
    }

    if (isLoss()) LossPostActions();
}

void Kingdom::AddCastle(const Castle *castle)
{
    if (castle)
    {
        if (castles.end() == find(castles.begin(), castles.end(), castle))
            castles.push_back(const_cast<Castle *>(castle));

        Player *player = Settings::Get().GetPlayers().GetCurrent();
        if (player && player->isColor(GetColor()))
            Interface::Basic::Get().GetIconsPanel().ResetIcons(ICON_CASTLES);

        AI::CastleAdd(*castle);
    }

    lost_town_days = Game::GetLostTownDays() + 1;
}

void Kingdom::RemoveCastle(const Castle *castle)
{
    if (castle)
    {
        if (!castles.empty())
            castles.erase(find(castles.begin(), castles.end(), castle));

        AI::CastleRemove(*castle);
    }

    if (isLoss()) LossPostActions();
}

u32 Kingdom::GetCountCastle() const
{
    return count_if(castles.begin(), castles.end(), Castle::PredicateIsCastle);
}

u32 Kingdom::GetCountTown() const
{
    return count_if(castles.begin(), castles.end(), Castle::PredicateIsTown);
}

u32 Kingdom::GetCountMarketplace() const
{
    return count_if(castles.begin(), castles.end(), Castle::PredicateIsBuildMarketplace);
}

u32 Kingdom::GetCountNecromancyShrineBuild() const
{
    return count_if(castles.begin(), castles.end(), mem_fun(&Castle::isNecromancyShrineBuild));
}

u32 Kingdom::GetCountBuilding(u32 build) const
{
    return count_if(castles.begin(), castles.end(), bind2nd(mem_fun(&Castle::isBuild), build));
}

bool Kingdom::AllowPayment(const Funds &funds) const
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
bool Kingdom::isVisited(const Maps::Tiles &tile) const
{
    return isVisited(tile.GetIndex(), tile.GetObject());
}

bool Kingdom::isVisited(s32 index, int object) const
{
    auto it = find_if(visit_object.begin(), visit_object.end(),
                      bind2nd(mem_fun_ref(&IndexObject::isIndex),
                              index));
    return visit_object.end() != it && (*it).isObject(object);
}

/* return true if object visited */
bool Kingdom::isVisited(int object) const
{
    return visit_object.end() != find_if(visit_object.begin(), visit_object.end(),
                                         bind2nd(mem_fun_ref(&IndexObject::isObject), object));
}

u32 Kingdom::CountVisitedObjects(int object) const
{
    return count_if(visit_object.begin(), visit_object.end(),
                    bind2nd(mem_fun_ref(&IndexObject::isObject), object));
}

/* set visited cell */
void Kingdom::SetVisited(s32 index, int object)
{
    if (!isVisited(index, object) && object != MP2::OBJ_ZERO) visit_object.push_front(IndexObject(index, object));
}

bool Kingdom::HeroesMayStillMove() const
{
    return heroes.end() != find_if(heroes.begin(), heroes.end(), mem_fun(&Heroes::MayStillMove));
}

u32 Kingdom::GetCountCapital() const
{
    return count_if(castles.begin(), castles.end(), Castle::PredicateIsCapital);
}

void Kingdom::AddFundsResource(const Funds &funds)
{
    resource = resource + funds;
}

void Kingdom::OddFundsResource(const Funds &funds)
{
    resource = resource - funds;
}

u32 Kingdom::GetLostTownDays() const
{
    return lost_town_days;
}

Recruits &Kingdom::GetRecruits()
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

const Puzzle &Kingdom::PuzzleMaps() const
{
    return puzzle_maps;
}

Puzzle &Kingdom::PuzzleMaps()
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
    return (heroes.size() < GetMaxHeroes()) && (!check_payment || AllowPayment(PaymentConditions::RecruitHero(level)));
}

void Kingdom::ApplyPlayWithStartingHero()
{
    if (isPlay() && !castles.empty())
    {
        // get first castle
        Castle *first = castles.GetFirstCastle();
        if (nullptr == first) first = castles.front();

        // check manual set hero (castle position + point(0, 1))?
        const Point &cp = (first)->GetCenter();
        Heroes *hero = world.GetTiles(cp.x, cp.y + 1).GetHeroes();

        // and move manual set hero to castle
        if (hero && hero->GetColor() == GetColor())
        {
            bool patrol = hero->Modes(Heroes::PATROL);
            hero->SetFreeman(0);
            hero->Recruit(*first);

            if (patrol)
            {
                hero->SetModes(Heroes::PATROL);
                hero->SetCenterPatrol(cp);
            }
        } else if (Settings::Get().GameStartWithHeroes())
        {
            Heroes *hero = world.GetFreemanHeroes(first->GetRace());
            if (hero && AllowRecruitHero(false, 0)) hero->Recruit(*first);
        }
    }
}

u32 Kingdom::GetMaxHeroes()
{
    return GameStatic::GetKingdomMaxHeroes();
}

void Kingdom::HeroesActionNewPosition()
{
    // Heroes::ActionNewPosition: can remove elements from heroes vector.
    KingdomHeroes heroes2(heroes);
    for_each(heroes2.begin(), heroes2.end(), mem_fun(&Heroes::ActionNewPosition));
}

Funds Kingdom::GetIncome(int type /* INCOME_ALL */) const
{
    Funds resource;

    if (INCOME_CAPTURED & type)
    {
        // captured object
        const int resources[] = {Resource::WOOD, Resource::ORE, Resource::MERCURY, Resource::SULFUR,
                                 Resource::CRYSTAL, Resource::GEMS, Resource::GOLD, Resource::UNKNOWN};

        for (u32 index = 0; resources[index] != Resource::UNKNOWN; ++index)
            resource += ProfitConditions::FromMine(resources[index]) *
                        world.CountCapturedMines(resources[index], GetColor());
    }

    if (INCOME_CASTLES & type)
    {
        // castles
        for (auto it : castles)
        {
            const Castle &castle = *it;

            // castle or town profit
            resource += ProfitConditions::FromBuilding((castle.isCastle() ? BUILD_CASTLE : BUILD_TENT), 0);

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
        const int artifacts[] = {Artifact::GOLDEN_GOOSE, Artifact::ENDLESS_SACK_GOLD, Artifact::ENDLESS_BAG_GOLD,
                                 Artifact::ENDLESS_PURSE_GOLD, Artifact::ENDLESS_POUCH_SULFUR,
                                 Artifact::ENDLESS_VIAL_MERCURY,
                                 Artifact::ENDLESS_POUCH_GEMS, Artifact::ENDLESS_CORD_WOOD, Artifact::ENDLESS_CART_ORE,
                                 Artifact::ENDLESS_POUCH_CRYSTAL, Artifact::UNKNOWN};

        for (u32 index = 0; artifacts[index] != Artifact::UNKNOWN; ++index)
            for (auto heroe : heroes)
                resource += ProfitConditions::FromArtifact(artifacts[index]) *
                            (*heroe).GetBagArtifacts().Count(Artifact(artifacts[index]));

        // TAX_LIEN
        for (auto heroe : heroes)
            resource -= ProfitConditions::FromArtifact(Artifact::TAX_LIEN) *
                        (*heroe).GetBagArtifacts().Count(Artifact(Artifact::TAX_LIEN));
    }

    if (INCOME_HEROSKILLS & type)
    {
        // estates skill bonus
        for (auto heroe : heroes)
            resource.gold += (*heroe).GetSecondaryValues(Skill::Secondary::ESTATES);
    }

    return resource;
}

const Heroes *Kingdom::GetBestHero() const
{
    return !heroes.empty() ? *max_element(heroes.begin(), heroes.end(), HeroesStrongestArmy) : nullptr;
}

u32 Kingdom::GetArmiesStrength() const
{
    u32 res = 0;

    for (auto heroe : heroes)
        res += (*heroe).GetArmy().m_troops.GetStrength();

    for (auto castle : castles)
        res += (*castle).GetArmy().m_troops.GetStrength();

    return res;
}

Kingdoms::Kingdoms()
= default;

void Kingdoms::Init()
{
    const Colors colors(Settings::Get().GetPlayers().GetColors());

    clear();

    for (int color : colors)
        GetKingdom(color).Init(color);
}

u32 Kingdoms::size() const
{
    return KINGDOMMAX + 1;
}

void Kingdoms::clear()
{
    for (u32 ii = 0; ii < size(); ++ii)
        kingdoms[ii].clear();
}

void Kingdoms::ApplyPlayWithStartingHero()
{
    for (u32 ii = 0; ii < size(); ++ii)
        if (kingdoms[ii].isPlay()) kingdoms[ii].ApplyPlayWithStartingHero();
}

const Kingdom &Kingdoms::GetKingdom(int color) const
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

Kingdom &Kingdoms::GetKingdom(int color)
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

void Kingdom::SetLastLostHero(Heroes &hero)
{
    lost_hero.first = hero.GetID();
    lost_hero.second = world.CountDay();
}

void Kingdom::ResetLastLostHero()
{
    lost_hero.first = Heroes::UNKNOWN;
    lost_hero.second = 0;
}

Heroes *Kingdom::GetLastLostHero() const
{
    return Heroes::UNKNOWN != lost_hero.first && world.CountDay() - lost_hero.second < DAYOFWEEK ?
           world.GetHeroes(lost_hero.first) : nullptr;
}

void Kingdoms::NewDay()
{
    for (u32 ii = 0; ii < size(); ++ii)
        if (kingdoms[ii].isPlay()) kingdoms[ii].ActionNewDay();
}

void Kingdoms::NewWeek()
{
    for (u32 ii = 0; ii < size(); ++ii)
        if (kingdoms[ii].isPlay()) kingdoms[ii].ActionNewWeek();
}

void Kingdoms::NewMonth()
{
    for (u32 ii = 0; ii < size(); ++ii)
        if (kingdoms[ii].isPlay()) kingdoms[ii].ActionNewMonth();
}

int Kingdoms::GetNotLossColors() const
{
    int result = 0;
    for (u32 ii = 0; ii < size(); ++ii)
        if (kingdoms[ii].GetColor() &&
            !kingdoms[ii].isLoss())
            result |= kingdoms[ii].GetColor();
    return result;
}

int Kingdoms::GetLossColors() const
{
    int result = 0;
    for (u32 ii = 0; ii < size(); ++ii)
        if (kingdoms[ii].GetColor() &&
            kingdoms[ii].isLoss())
            result |= kingdoms[ii].GetColor();
    return result;
}

int Kingdoms::FindWins(int cond) const
{
    for (u32 ii = 0; ii < size(); ++ii)
        if (kingdoms[ii].GetColor() &&
            world.KingdomIsWins(kingdoms[ii], cond))
            return kingdoms[ii].GetColor();
    return 0;
}

void Kingdoms::AddHeroes(const AllHeroes &heroes)
{
    for (auto heroe : heroes)
        // skip gray color
        if (heroe->GetColor()) GetKingdom(heroe->GetColor()).AddHeroes(heroe);
}

void Kingdoms::AddCondLossHeroes(const AllHeroes &heroes)
{
    for (auto heroe : heroes)
        // skip gray color
        if (heroe->GetColor())
        {
            Kingdom &kingdom = GetKingdom(heroe->GetColor());

            if (kingdom.isControlHuman())
            {
                heroe->SetModes(Heroes::NOTDISMISS | Heroes::NOTDEFAULTS);
                kingdom.AddHeroStartCondLoss(heroe);
            }
        }
}

void Kingdoms::AddCastles(const AllCastles &castles)
{
    for (auto castle : castles)
        // skip gray color
        if (castle->GetColor()) GetKingdom(castle->GetColor()).AddCastle(castle);
}

void Kingdoms::AddTributeEvents(CapturedObjects &captureobj, u32 day, int obj)
{
    for (u32 ii = 0; ii < size(); ++ii)
        if (kingdoms[ii].isPlay())
        {
            const int color = kingdoms[ii].GetColor();
            const Funds &funds = captureobj.TributeCapturedObject(color, obj);

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

StreamBase &operator<<(StreamBase &msg, const Kingdom &kingdom)
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

ByteVectorReader &operator>>(ByteVectorReader &msg, Kingdom &kingdom)
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


StreamBase &operator<<(StreamBase &msg, const Kingdoms &obj)
{
    msg << static_cast<u32>(obj.size());
    for (u32 ii = 0; ii < obj.size(); ++ii)
        msg << obj.kingdoms[ii];

    return msg;
}


ByteVectorReader &operator>>(ByteVectorReader &msg, Kingdoms &obj)
{
    u32 kingdomscount;
    msg >> kingdomscount; // FIXME: check kingdomscount

    for (u32 ii = 0; ii < kingdomscount; ++ii)
        msg >> obj.kingdoms[ii];

    return msg;
}

StreamBase &operator>>(StreamBase &sb, LastLoseHero &st)
{
    return sb >> st.first >> st.second;
}

KingdomHeroes& KingdomHeroes::GetNull()
{
    static KingdomHeroes nullInstance;
    return nullInstance;
}
