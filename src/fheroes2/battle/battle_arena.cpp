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

#include <algorithm>
#include "settings.h"
#include "army.h"
#include "cursor.h"
#include "castle.h"
#include "world.h"
#include "agg.h"
#include "ground.h"
#include "race.h"
#include "battle_arena.h"
#include "battle_army.h"
#include "battle_troop.h"
#include "battle_tower.h"
#include "battle_catapult.h"
#include "battle_bridge.h"
#include "battle_interface.h"
#include "battle_command.h"
#include "localevent.h"
#include "tools.h"
#include "m82.h"
#include "mus.h"
#include "rand.h"

#include "icn.h"
#include "audio_mixer.h"
#include "audio_music.h"

namespace Battle
{
    Arena* arena = nullptr;
}

int GetCovr(int ground)
{
    vector<int> covrs;

    switch (ground)
    {
    case Maps::Ground::SNOW:
        covrs.push_back(ICN::COVR0007);
        covrs.push_back(ICN::COVR0008);
        covrs.push_back(ICN::COVR0009);
        covrs.push_back(ICN::COVR0010);
        covrs.push_back(ICN::COVR0011);
        covrs.push_back(ICN::COVR0012);
        break;

    case Maps::Ground::WASTELAND:
        covrs.push_back(ICN::COVR0019);
        covrs.push_back(ICN::COVR0020);
        covrs.push_back(ICN::COVR0021);
        covrs.push_back(ICN::COVR0022);
        covrs.push_back(ICN::COVR0023);
        covrs.push_back(ICN::COVR0024);
        break;

    case Maps::Ground::DIRT:
        covrs.push_back(ICN::COVR0013);
        covrs.push_back(ICN::COVR0014);
        covrs.push_back(ICN::COVR0015);
        covrs.push_back(ICN::COVR0016);
        covrs.push_back(ICN::COVR0017);
        covrs.push_back(ICN::COVR0018);
        break;

    case Maps::Ground::GRASS:
        covrs.push_back(ICN::COVR0001);
        covrs.push_back(ICN::COVR0002);
        covrs.push_back(ICN::COVR0003);
        covrs.push_back(ICN::COVR0004);
        covrs.push_back(ICN::COVR0005);
        covrs.push_back(ICN::COVR0006);
        break;

    default:
        break;
    }

    return covrs.empty() ? ICN::UNKNOWN : *Rand::Get(covrs);
}

ByteVectorWriter& Battle::operator<<(ByteVectorWriter& msg, const TargetInfo& t)
{
    return msg <<
        (t.defender ? t.defender->GetUID() : static_cast<uint32_t>(0)) <<
        t.damage << t.killed << t.resist;
}

ByteVectorReader& Battle::operator>>(ByteVectorReader& msg, TargetInfo& t)
{
    uint32_t uid = 0;

    msg >> uid >>
        t.damage >> t.killed >> t.resist;

    t.defender = uid ? GetArena()->GetTroopUID(uid) : nullptr;

    return msg;
}

ByteVectorWriter& Battle::operator<<(ByteVectorWriter& msg, const TargetsInfo& ts)
{
    msg << static_cast<uint32_t>(ts.size());

    for (const auto& t : ts)
        msg << t;

    return msg;
}

ByteVectorReader& Battle::operator>>(ByteVectorReader& msg, TargetsInfo& ts)
{
    uint32_t size = 0;

    msg >> size;
    ts.resize(size);

    for (auto& t : ts)
        msg >> t;

    return msg;
}

bool Battle::TargetInfo::operator==(const TargetInfo& ta) const
{
    return defender == ta.defender;
}

Battle::Arena* Battle::GetArena()
{
    return arena;
}

const Castle* Battle::Arena::GetCastle()
{
    return arena->castle;
}

Battle::Bridge* Battle::Arena::GetBridge()
{
    return arena->bridge;
}

Battle::Board* Battle::Arena::GetBoard()
{
    return &arena->board;
}

Battle::Graveyard* Battle::Arena::GetGraveyard()
{
    return &arena->graveyard;
}

Battle::Interface* Battle::Arena::GetInterface()
{
    return arena->interface.get();
}

Battle::Tower* Battle::Arena::GetTower(int type)
{
    switch (type)
    {
    case TWR_LEFT:
        return arena->towers[0].get();
    case TWR_CENTER:
        return arena->towers[1].get();
    case TWR_RIGHT:
        return arena->towers[2].get();
    default:
        break;
    }
    return nullptr;
}

Battle::Arena::Arena(Army& a1, Army& a2, s32 index, bool local) :
    army1(nullptr), army2(nullptr), armies_order(nullptr), castle(nullptr), current_color(0), catapult(nullptr),
    bridge(nullptr), interface(nullptr), icn_covr(ICN::UNKNOWN), current_turn(0), auto_battle(0), end_turn(false)
{
    const Settings& conf = Settings::Get();
    auto_battle = conf.QuickCombat();
    usage_spells.reserve(20);

    arena = this;
    army1 = make_unique<Force>(a1, false);
    army2 = make_unique<Force>(a2, true);

    // init castle (interface ahead)
    castle = world.GetCastle(Maps::GetPoint(index));

    if (castle)
    {
        CastleHeroes heroes = world.GetHeroes(*castle);

        // skip if present guard and guest
        if (heroes.FullHouse()) castle = nullptr;

        // skip for town
        if (castle && !castle->isCastle()) castle = nullptr;
    }

    // init interface
    if (local && !conf.QuickCombat())
    {
        interface = std::make_unique<Interface>(*this, index);
        board.SetArea(interface->GetArea());

        for_each(army1->_items.begin(), army1->_items.end(), mem_fun(&Unit::InitContours));
        for_each(army2->_items.begin(), army2->_items.end(), mem_fun(&Unit::InitContours));

        if (conf.Sound())
            AGG::PlaySound(M82::PREBATTL);

        armies_order = std::make_unique<Units>();
        armies_order->_items.reserve(25);
        interface->SetArmiesOrder(armies_order.get());
    }


    towers[0] = nullptr;
    towers[1] = nullptr;
    towers[2] = nullptr;

    if (castle)
    {
        // init
        towers[0] = castle->isBuild(BUILD_LEFTTURRET)
                        ? std::make_unique<Tower>(*castle, TWR_LEFT)
                        : nullptr;
        towers[1] = std::make_unique<Tower>(*castle, TWR_CENTER);
        towers[2] = castle->isBuild(BUILD_RIGHTTURRET)
                        ? std::make_unique<Tower>(*castle, TWR_RIGHT)
                        : nullptr;
        bool fortification = Race::KNGT == castle->GetRace() && castle->isBuild(BUILD_SPEC);
        catapult = army1->GetCommander()
                       ? std::make_unique<Catapult>(*army1->GetCommander(), fortification)
                       : nullptr;
        bridge = new Bridge();

        // catapult cell
        board._items[77].SetObject(1);

        // wall (3,2,1,0)
        board._items[8].SetObject(fortification ? 3 : 2);
        board._items[29].SetObject(fortification ? 3 : 2);
        board._items[73].SetObject(fortification ? 3 : 2);
        board._items[96].SetObject(fortification ? 3 : 2);

        // tower
        board._items[40].SetObject(2);
        board._items[62].SetObject(2);

        // archers tower
        board._items[19].SetObject(2);
        board._items[85].SetObject(2);

        // bridge
        board._items[49].SetObject(1);
        board._items[50].SetObject(1);
    }
    else
        // set obstacles
    {
        MapsIndexes mapIndexes;
        Maps::ScanAroundObject(index, MP2::OBJ_CRATER, mapIndexes);
        icn_covr = !mapIndexes.empty() ? GetCovr(world.GetTiles(index).GetGround()) : ICN::UNKNOWN;

        if (icn_covr != ICN::UNKNOWN)
            board.SetCovrObjects(icn_covr);
        else
            board.SetCobjObjects(world.GetTiles(index));
    }

    // set guardian objects mode (+2 defense)
    if (conf.ExtWorldGuardianObjectsTwoDefense() &&
        !castle &&
        MP2::isCaptureObject(world.GetTiles(index).GetObject(false)))
        army2->SetModes(ARMY_GUARDIANS_OBJECT);

    //
    if (interface)
    {
        Cursor& cursor = Cursor::Get();
        Display& display = Display::Get();

        cursor.Hide();
        cursor.SetThemes(Cursor::WAR_NONE);

        if (conf.ExtGameUseFade()) display.Fade();

        interface->Redraw();
        cursor.Show();
        display.Flip();

        // pause for play M82::PREBATTL
        if (conf.Sound())
            while (LocalEvent::Get().HandleEvents() && Mixer::isPlaying(-1));
    }
}

Battle::Arena::~Arena() = default;

void Battle::Arena::TurnTroop(Unit* current_troop)
{
    Actions actions;
    end_turn = false;

    while (!end_turn)
    {
        // bad morale
        if (current_troop->Modes(MORALE_BAD))
        {
            actions.push_back(Command(MSG_BATTLE_MORALE, current_troop->GetUID(), false));
            end_turn = true;
        }
        else
        {
            // turn opponents
            if (current_troop->isControlRemote())
                RemoteTurn(*current_troop, actions);
            else
            {
                if (current_troop->isControlAI() ||
                    current_color & auto_battle)
                {
                    AI::BattleTurn(*this, *current_troop, actions);
                }
                else if (current_troop->isControlHuman())
                    HumanTurn(*current_troop, actions);
            }
        }

        // apply task
        while (!actions.empty())
        {
            // apply action
            ApplyAction(actions.front());
            actions.pop_front();

            // rescan orders
            if (armies_order)
                Force::UpdateOrderUnits(*army1, *army2, *armies_order);

            // check end battle
            if (!BattleValid())
                end_turn = true;

            // good morale
            if (!end_turn &&
                !current_troop->Modes(TR_SKIPMOVE) &&
                current_troop->Modes(TR_MOVED) &&
                current_troop->Modes(MORALE_GOOD) &&
                BattleValid())
            {
                actions.push_back(Command(MSG_BATTLE_MORALE, current_troop->GetUID(), true));
                end_turn = false;
            }
        }

        if (current_troop->Modes(TR_SKIPMOVE | TR_MOVED))
            end_turn = true;

        board.Reset();

        DELAY(10);
    }
}

bool Battle::Arena::BattleValid() const
{
    return army1->isValid() && army2->isValid() &&
        0 == result_game.army1 && 0 == result_game.army2;
}

void Battle::Arena::Turns()
{
    const Settings& conf = Settings::Get();

    ++current_turn;

    if (interface && conf.Music() && !Music::isPlaying())
        AGG::PlayMusic(MUS::GetBattleRandom(), false);

    army1->NewTurn();
    army2->NewTurn();

    bool tower_moved = false;
    bool catapult_moved = false;

    Unit* current_troop = nullptr;

    // rescan orders
    if (armies_order) Force::UpdateOrderUnits(*army1, *army2, *armies_order);

    while (BattleValid() &&
        nullptr != (current_troop = Force::GetCurrentUnit(*army1, *army2, current_troop, true)))
    {
        current_color = current_troop->GetArmyColor();

        // first turn: castle and catapult action
        if (castle)
        {
            if (!catapult_moved && current_troop->GetColor() == army1->GetColor())
            {
                CatapultAction();
                catapult_moved = true;
            }

            if (!tower_moved && current_troop->GetColor() == army2->GetColor())
            {
                if (towers[0] && towers[0]->isValid()) TowerAction(*towers[0]);
                if (towers[1] && towers[1]->isValid()) TowerAction(*towers[1]);
                if (towers[2] && towers[2]->isValid()) TowerAction(*towers[2]);
                tower_moved = true;

                // check dead last army from towers
                if (!BattleValid()) break;
            }
        }

        // set bridge passable
        if (bridge && bridge->isValid() && !bridge->isDown()) bridge->SetPassable(*current_troop);

        // turn troop
        TurnTroop(current_troop);
    }

    current_troop = nullptr;

    // can skip move ?
    if (Settings::Get().ExtBattleSoftWait())
        while (BattleValid() &&
            nullptr != (current_troop = Force::GetCurrentUnit(*army1, *army2, current_troop, false)))
        {
            current_color = current_troop->GetArmyColor();

            // set bridge passable
            if (bridge && bridge->isValid() && !bridge->isDown()) bridge->SetPassable(*current_troop);

            // turn troop
            TurnTroop(current_troop);
        }

    // end turn: fix result
    if (!army1->isValid() || result_game.army1 & (RESULT_RETREAT | RESULT_SURRENDER))
    {
        result_game.army1 |= RESULT_LOSS;
        if (army2->isValid()) result_game.army2 = RESULT_WINS;
    }

    if (!army2->isValid() || result_game.army2 & (RESULT_RETREAT | RESULT_SURRENDER))
    {
        result_game.army2 |= RESULT_LOSS;
        if (army1->isValid()) result_game.army1 = RESULT_WINS;
    }

    // fix experience and killed
    if (result_game.army1 || result_game.army2)
    {
        result_game.exp1 = army2->GetDeadHitPoints();
        result_game.exp2 = army1->GetDeadHitPoints();

        if (army1->GetCommander()) result_game.exp2 += 500;
        if (army2->GetCommander()) result_game.exp1 += 500;

        Force* army_loss = result_game.army1 & RESULT_LOSS
                               ? army1.get()
                               : result_game.army2 & RESULT_LOSS
                               ? army2.get()
                               : nullptr;
        result_game.killed = army_loss ? army_loss->GetDeadCounts() : 0;
    }
}

void Battle::Arena::RemoteTurn(const Unit& b, Actions& a)
{
    AI::BattleTurn(*this, b, a);
}

void Battle::Arena::HumanTurn(const Unit& b, Actions& a)
{
    if (Settings::Get().QuickCombat())
    {
        AI::BattleTurn(*this, b, a);
        return;
    }
    if (interface)
        interface->HumanTurn(b, a);
}

void Battle::Arena::TowerAction(const Tower& twr)
{
    board.Reset();
    board.SetEnemyQuality(twr);
    const Unit* enemy = GetEnemyMaxQuality(twr.GetColor());

    if (enemy)
    {
        Command cmd(MSG_BATTLE_TOWER, twr.GetType(), enemy->GetUID());
        ApplyAction(cmd);
    }
}

void Battle::Arena::CatapultAction()
{
    if (catapult)
    {
        uint32_t shots = catapult->GetShots();
        vector<uint32_t> values(CAT_MISS + 1, 0);

        values[CAT_WALL1] = GetCastleTargetValue(CAT_WALL1);
        values[CAT_WALL2] = GetCastleTargetValue(CAT_WALL2);
        values[CAT_WALL3] = GetCastleTargetValue(CAT_WALL3);
        values[CAT_WALL4] = GetCastleTargetValue(CAT_WALL4);
        values[CAT_TOWER1] = GetCastleTargetValue(CAT_TOWER1);
        values[CAT_TOWER2] = GetCastleTargetValue(CAT_TOWER2);
        values[CAT_TOWER3] = GetCastleTargetValue(CAT_TOWER3);
        values[CAT_BRIDGE] = GetCastleTargetValue(CAT_BRIDGE);

        Command cmd(MSG_BATTLE_CATAPULT);

        while (shots--)
        {
            int target = catapult->GetTarget(values);
            uint32_t damage = catapult->GetDamage(target, GetCastleTargetValue(target));
            cmd << damage << target;
            values[target] -= damage;
        }

        cmd << catapult->GetShots();
        ApplyAction(cmd);
    }
}

Battle::Indexes Battle::Arena::GetPath(const Unit& b, const Position& dst)
{
    Indexes result = board.GetAStarPath(b, dst);

    return result;
}

Battle::Unit* Battle::Arena::GetTroopBoard(s32 index)
{
    return Board::isValidIndex(index) ? board._items[index].GetUnit() : nullptr;
}

const Battle::Unit* Battle::Arena::GetTroopBoard(s32 index) const
{
    return Board::isValidIndex(index) ? board._items[index].GetUnit() : nullptr;
}

const HeroBase* Battle::Arena::GetCommander1() const
{
    return army1->GetCommander();
}

const HeroBase* Battle::Arena::GetCommander2() const
{
    return army2->GetCommander();
}

int Battle::Arena::GetArmyColor1() const
{
    return army1->GetColor();
}

int Battle::Arena::GetArmyColor2() const
{
    return army2->GetColor();
}

int Battle::Arena::GetCurrentColor() const
{
    return current_color;
}

int Battle::Arena::GetOppositeColor(int col) const
{
    return col == GetArmyColor1() ? GetArmyColor2() : GetArmyColor1();
}

Battle::Unit* Battle::Arena::GetTroopUID(uint32_t uid)
{
    auto it = find_if(army1->_items.begin(), army1->_items.end(),
                      bind2nd(mem_fun(&Unit::isUID), uid));

    if (it != army1->_items.end()) return *it;

    it = find_if(army2->_items.begin(), army2->_items.end(),
                 bind2nd(mem_fun(&Unit::isUID), uid));

    return it != army2->_items.end() ? *it : nullptr;
}

const Battle::Unit* Battle::Arena::GetTroopUID(uint32_t uid) const
{
    auto it = find_if(army1->_items.begin(), army1->_items.end(),
                      bind2nd(mem_fun(&Unit::isUID), uid));

    if (it != army1->_items.end()) return *it;

    it = find_if(army2->_items.begin(), army2->_items.end(),
                 bind2nd(mem_fun(&Unit::isUID), uid));

    return it != army2->_items.end() ? *it : nullptr;
}

const Battle::Unit* Battle::Arena::GetEnemyMaxQuality(int my_color) const
{
    const Unit* res = nullptr;
    s32 quality = 0;

    for (const auto& it : board._items)
    {
        const Unit* enemy = it.GetUnit();

        if (enemy && enemy->GetColor() != my_color &&
            (!enemy->isWide() || enemy->GetTailIndex() != it.GetIndex()) &&
            quality < it.GetQuality())
        {
            res = enemy;
            quality = it.GetQuality();
        }
    }

    return res;
}

void Battle::Arena::FadeArena() const
{
    if (interface) interface->FadeArena();
}

const SpellStorage& Battle::Arena::GetUsageSpells() const
{
    return usage_spells;
}

s32 Battle::Arena::GetFreePositionNearHero(int color) const
{
    const int cells1[] = {11, 22, 33};
    const int cells2[] = {21, 32, 43};
    const int* cells = nullptr;

    if (army1->GetColor() == color) cells = cells1;
    else if (army2->GetColor() == color) cells = cells2;

    if (!cells)
    {
        return -1;
    }
    for (uint32_t ii = 0; ii < 3; ++ii)
    {
        if (board._items[cells[ii]].isPassable1(true)
            && nullptr == board._items[cells[ii]].GetUnit())
        {
            return cells[ii];
        }
    }
    return -1;
}

bool Battle::Arena::CanSurrenderOpponent(int color) const
{
    const HeroBase* hero1 = GetCommander(color, false); // enemy
    const HeroBase* hero2 = GetCommander(color, true);
    return hero1 && hero1->isHeroes() && hero2 &&
        !world.GetKingdom(hero2->GetColor()).GetCastles().empty();
}

bool Battle::Arena::CanRetreatOpponent(int color) const
{
    const HeroBase* hero = army1->GetColor() == color ? army1->GetCommander() : army2->GetCommander();
    return hero && hero->isHeroes() && nullptr == hero->inCastle() &&
        !world.GetKingdom(hero->GetColor()).GetCastles().empty();
}

bool Battle::Arena::isDisableCastSpell(const Spell& spell, string* msg)
{
    const HeroBase* hero1 = army1->GetCommander();
    const HeroBase* hero2 = army2->GetCommander();
    const HeroBase* current_commander = GetCurrentCommander();

    // check sphere negation (only for heroes)
    if (hero1 && hero1->HasArtifact(Artifact::SPHERE_NEGATION) ||
        hero2 && hero2->HasArtifact(Artifact::SPHERE_NEGATION))
    {
        if (msg) *msg = _("The Sphere of Negation artifact is in effect for this battle, disabling all combat spells.");
        return true;
    }

    // check casted
    if (current_commander)
    {
        if (current_commander->Modes(Heroes::SPELLCASTED))
        {
            if (msg) *msg = _("You have already cast a spell this round.");
            return true;
        }

        if (spell == Spell::EARTHQUAKE && !castle)
        {
            *msg = _("That spell will affect no one!");
            return true;
        }
        if (spell.isSummon())
        {
            const Unit* elem = GetCurrentForce().FindMode(CAP_SUMMONELEM);
            bool affect = true;

            if (elem)
                switch (spell())
                {
                case Spell::SUMMONEELEMENT:
                    if (elem->GetID() != Monster::EARTH_ELEMENT) affect = false;
                    break;
                case Spell::SUMMONAELEMENT:
                    if (elem->GetID() != Monster::AIR_ELEMENT) affect = false;
                    break;
                case Spell::SUMMONFELEMENT:
                    if (elem->GetID() != Monster::FIRE_ELEMENT) affect = false;
                    break;
                case Spell::SUMMONWELEMENT:
                    if (elem->GetID() != Monster::WATER_ELEMENT) affect = false;
                    break;
                default:
                    break;
                }
            if (!affect)
            {
                *msg = _("You may only summon one type of elemental per combat.");
                return true;
            }

            if (0 > GetFreePositionNearHero(current_color))
            {
                *msg = _("There is no open space adjacent to your hero to summon an Elemental to.");
                return true;
            }
        }
        else if (spell.isValid())
        {
            // check army
            for (auto it = board._items.begin(); it != board._items.end(); ++it)
            {
                const Unit* b = (*it).GetUnit();

                if (b)
                {
                    if (b->AllowApplySpell(spell, current_commander, nullptr))
                        return false;
                }
                else
                    // check graveyard
                    if (GraveyardAllowResurrect((*it).GetIndex(), spell))
                        return false;
            }
            *msg = _("That spell will affect no one!");
            return true;
        }
    }

    // may be check other..
    /*
    */

    return false;
}

bool Battle::Arena::GraveyardAllowResurrect(s32 index, const Spell& spell) const
{
    const HeroBase* hero = GetCurrentCommander();
    const Unit* killed = GetTroopUID(graveyard.GetLastTroopUID(index));
    const Unit* tail = killed && killed->isWide()
                           ? GetTroopUID(graveyard.GetLastTroopUID(killed->GetTailIndex()))
                           : nullptr;

    return killed && (!killed->isWide() || killed == tail) &&
        hero && spell.isResurrect() && killed->AllowApplySpell(spell, hero, nullptr);
}

const Battle::Unit* Battle::Arena::GraveyardLastTroop(s32 index) const
{
    return GetTroopUID(graveyard.GetLastTroopUID(index));
}

Battle::Indexes Battle::Arena::GraveyardClosedCells() const
{
    return graveyard.GetClosedCells();
}

void Battle::Arena::SetCastleTargetValue(int target, uint32_t value)
{
    switch (target)
    {
    case CAT_WALL1:
        board._items[8].SetObject(value);
        break;
    case CAT_WALL2:
        board._items[29].SetObject(value);
        break;
    case CAT_WALL3:
        board._items[73].SetObject(value);
        break;
    case CAT_WALL4:
        board._items[96].SetObject(value);
        break;

    case CAT_TOWER1:
        if (towers[0] && towers[0]->isValid()) towers[0]->SetDestroy();
        break;
    case CAT_TOWER2:
        if (towers[2] && towers[2]->isValid()) towers[2]->SetDestroy();
        break;
    case CAT_TOWER3:
        if (towers[1] && towers[1]->isValid()) towers[1]->SetDestroy();
        break;

    case CAT_BRIDGE:
        if (bridge->isValid())
        {
            if (interface) interface->RedrawBridgeAnimation(true);
            bridge->SetDown(true);
            bridge->SetDestroy();
        }
        break;

    default:
        break;
    }
}

uint32_t Battle::Arena::GetCastleTargetValue(int target) const
{
    switch (target)
    {
    case CAT_WALL1:
        return board._items[8].GetObject();
    case CAT_WALL2:
        return board._items[29].GetObject();
    case CAT_WALL3:
        return board._items[73].GetObject();
    case CAT_WALL4:
        return board._items[96].GetObject();

    case CAT_TOWER1:
        return towers[0] && towers[0]->isValid();
    case CAT_TOWER2:
        return towers[2] && towers[2]->isValid();
    case CAT_TOWER3:
        return towers[1] && towers[1]->isValid();

    case CAT_BRIDGE:
        return bridge->isValid();

    default:
        break;
    }
    return 0;
}

vector<int> Battle::Arena::GetCastleTargets() const
{
    vector<int> targets;
    targets.reserve(8);

    // check walls
    if (0 != board._items[8].GetObject()) targets.push_back(CAT_WALL1);
    if (0 != board._items[29].GetObject()) targets.push_back(CAT_WALL2);
    if (0 != board._items[73].GetObject()) targets.push_back(CAT_WALL3);
    if (0 != board._items[96].GetObject()) targets.push_back(CAT_WALL4);

    // check right/left towers
    if (towers[0] && towers[0]->isValid()) targets.push_back(CAT_TOWER1);
    if (towers[2] && towers[2]->isValid()) targets.push_back(CAT_TOWER2);

    return targets;
}

ByteVectorWriter& Battle::operator<<(ByteVectorWriter& msg, const Arena& a)
{
    msg <<
        a.current_turn << a.board._items <<
        *a.army1 << *a.army2;

    const HeroBase* hero1 = a.army1->GetCommander();
    const HeroBase* hero2 = a.army2->GetCommander();

    if (hero1)
        msg << hero1->GetType() << *hero1;
    else
        msg << static_cast<int>(HeroBase::UNDEFINED);

    if (hero2)
        msg << hero2->GetType() << *hero2;
    else
        msg << static_cast<int>(HeroBase::UNDEFINED);

    return msg;
}

ByteVectorReader& Battle::operator>>(ByteVectorReader& msg, Arena& a)
{
    msg >> a.current_turn >> a.board._items >>
        *a.army1 >> *a.army2;

    int type;
    HeroBase* hero1 = a.army1->GetCommander();
    HeroBase* hero2 = a.army2->GetCommander();

    msg >> type;
    if (hero1 && type == hero1->GetType()) msg >> *hero1;

    msg >> type;
    if (hero2 && type == hero2->GetType()) msg >> *hero2;

    return msg;
}

const HeroBase* Battle::Arena::GetCommander(int color, bool invert) const
{
    const HeroBase* commander = nullptr;

    if (army1->GetColor() == color)
    {
        commander = invert ? army2->GetCommander() : army1->GetCommander();
    }
    else
    {
        commander = invert ? army1->GetCommander() : army2->GetCommander();
    }

    return commander;
}

const HeroBase* Battle::Arena::GetCurrentCommander() const
{
    return GetCommander(current_color);
}

bool Battle::Arena::NetworkTurn()
{
    return interface && interface->NetworkTurn(result_game);
}

Battle::Unit* Battle::Arena::CreateElemental(const Spell& spell) const
{
    const HeroBase* hero = GetCurrentCommander();
    const s32 pos = GetFreePositionNearHero(current_color);

    if (0 > pos || !hero)
    {
        return nullptr;
    }

    Force& army = GetCurrentForce();
    Unit* elem = army.FindMode(CAP_SUMMONELEM);
    bool affect = true;

    if (elem)
        switch (spell())
        {
        case Spell::SUMMONEELEMENT:
            if (elem->GetID() != Monster::EARTH_ELEMENT) affect = false;
            break;
        case Spell::SUMMONAELEMENT:
            if (elem->GetID() != Monster::AIR_ELEMENT) affect = false;
            break;
        case Spell::SUMMONFELEMENT:
            if (elem->GetID() != Monster::FIRE_ELEMENT) affect = false;
            break;
        case Spell::SUMMONWELEMENT:
            if (elem->GetID() != Monster::WATER_ELEMENT) affect = false;
            break;
        default:
            break;
        }

    if (!affect)
    {
        return nullptr;
    }

    Monster mons(spell);

    if (!mons.isValid())
    {
        return nullptr;
    }

    uint32_t count = spell.ExtraValue() * hero->GetPower();
    uint32_t acount = hero->HasArtifact(Artifact::BOOK_ELEMENTS);
    if (acount) count *= acount * 2;

    elem = new Unit(Troop(mons, count), pos, hero == army2->GetCommander());

    if (elem)
    {
        elem->SetModes(CAP_SUMMONELEM);
        elem->SetArmy(hero->GetArmy());
        if (interface) elem->InitContours();
        army._items.push_back(elem);
    }
    else
    {
    }

    return elem;
}

Battle::Unit* Battle::Arena::CreateMirrorImage(Unit& b, s32 pos) const
{
    auto* image = new Unit(b, pos, b.isReflect());

    b.SetMirror(image);
    image->SetArmy(*b.GetArmy());
    image->SetMirror(&b);
    image->SetModes(CAP_MIRRORIMAGE);
    if (interface) image->InitContours();
    b.SetModes(CAP_MIRROROWNER);

    GetCurrentForce()._items.push_back(image);


    return image;
}

uint32_t Battle::Arena::GetObstaclesPenalty(const Unit& attacker, const Unit& defender) const
{
    if (defender.Modes(CAP_TOWER) || attacker.Modes(CAP_TOWER)) return 0;

    // check golden bow artifact
    const HeroBase* enemy = attacker.GetCommander();
    if (enemy && enemy->HasArtifact(Artifact::GOLDEN_BOW)) return 0;

    uint32_t result = 0;
    const uint32_t step = CELLW / 3;

    if (castle)
    {
        // archery skill
        if (enemy && Skill::Level::NONE != enemy->GetLevelSkill(Skill::Secondary::ARCHERY)) return 0;

        // attacker is castle owner
        if (attacker.GetColor() == castle->GetColor() &&
            !attacker.OutOfWalls())
            return 0;

        if (defender.GetColor() == castle->GetColor() &&
            defender.OutOfWalls())
            return 0;

        // check castle walls defensed
        const Points points = GetLinePoints(attacker.GetBackPoint(), defender.GetBackPoint(), step);

        for (const auto point : points)
        {
            if (0 == board._items[8].GetObject() && board._items[8].GetPos() & point) return 0;
            if (0 == board._items[29].GetObject() && board._items[29].GetPos() & point) return 0;
            if (0 == board._items[73].GetObject() && board._items[73].GetPos() & point) return 0;
            if (0 == board._items[96].GetObject() && board._items[96].GetPos() & point) return 0;
        }

        result = 1;
    }
    else if (Settings::Get().ExtBattleObjectsArchersPenalty())
    {
        const Points points = GetLinePoints(attacker.GetBackPoint(), defender.GetBackPoint(), step);
        Indexes indexes;
        indexes.reserve(points.size());

        for (const auto point : points)
        {
            const s32 index = board.GetIndexAbsPosition(point);
            if (Board::isValidIndex(index)) indexes.push_back(index);
        }

        if (!indexes.empty())
        {
            sort(indexes.begin(), indexes.end());
            indexes.resize(distance(indexes.begin(), unique(indexes.begin(), indexes.end())));
        }

        for (auto it : indexes)
        {
            // obstacles
            switch (board._items[it].GetObject())
            {
                // tree
            case 0x82:
                // trock
            case 0x85:
                // tree
            case 0x89:
                // tree
            case 0x8D:
                // rock
            case 0x95:
            case 0x96:
                // stub
            case 0x9A:
                // dead tree
            case 0x9B:
                // tree
            case 0x9C:
                ++result;
                break;

            default:
                break;
            }
        }

        if (enemy)
        {
            switch (enemy->GetLevelSkill(Skill::Secondary::ARCHERY))
            {
            case Skill::Level::BASIC:
                if (result < 2) return 0;
                break;
            case Skill::Level::ADVANCED:
                if (result < 3) return 0;
                break;
            case Skill::Level::EXPERT:
                return 0;
            default:
                break;
            }
        }
    }

    return result;
}

Battle::Force& Battle::Arena::GetForce1() const
{
    return *army1;
}

Battle::Force& Battle::Arena::GetForce2() const
{
    return *army2;
}

Battle::Force& Battle::Arena::GetForce(int color, bool invert) const
{
    if (army1->GetColor() == color)
        return invert ? *army2 : *army1;

    return invert ? *army1 : *army2;
}

Battle::Force& Battle::Arena::GetCurrentForce() const
{
    return GetForce(current_color, false);
}

int Battle::Arena::GetICNCovr() const
{
    return icn_covr;
}

uint32_t Battle::Arena::GetCurrentTurn() const
{
    return current_turn;
}

Battle::Result& Battle::Arena::GetResult()
{
    return result_game;
}

bool Battle::Arena::CanBreakAutoBattle() const
{
    return auto_battle & current_color && GetCurrentCommander() &&
        !GetCurrentCommander()->isControlAI();
}

void Battle::Arena::BreakAutoBattle()
{
    auto_battle &= ~current_color;
}
