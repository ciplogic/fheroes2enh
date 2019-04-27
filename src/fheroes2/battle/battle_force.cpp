//
// Created by Ciprian Khlud on 2019-04-26.
//

#include "battle_force.h"
#include <algorithm>
#include "speed.h"
#include "settings.h"
#include "heroes.h"
#include "battle_troop.h"
#include "rand.h"
#include <functional>

namespace Battle
{
    bool AllowPart1(const Unit* b, bool f)
    {
        return (f ? !b->Modes(TR_SKIPMOVE) || b->Modes(TR_HARDSKIP) : !b->Modes(TR_SKIPMOVE)) &&
               Speed::STANDING != b->GetSpeed(f);
    }

    bool AllowPart2(const Unit* b, bool f)
    {
        return (f ? b->Modes(TR_SKIPMOVE) && !b->Modes(TR_HARDSKIP) : b->Modes(TR_SKIPMOVE)) &&
               Speed::STANDING != b->GetSpeed(f);
    }

    Unit* ForceGetCurrentUnitPart(Units& units1, Units& units2, bool part1, bool units1_first, bool orders_mode)
    {
        const auto it1 = part1
                         ? find_if(units1._items.begin(), units1._items.end(),
                                   [&](const Unit* unit) { return AllowPart1(unit, orders_mode); })
                         : find_if(units1._items.begin(), units1._items.end(),
                                   [&](const Unit* unit) { return AllowPart2(unit, orders_mode); });
        const auto it2 = part1
                         ? find_if(units2._items.begin(), units2._items.end(),
                                   [&](const Unit* unit) { return AllowPart1(unit, orders_mode); })
                         : find_if(units2._items.begin(), units2._items.end(),
                                   [&](const Unit* unit) { return AllowPart2(unit, orders_mode); });
        Unit* result = nullptr;

        if (it1 != units1._items.end() &&
            it2 != units2._items.end())
        {
            if ((*it1)->GetSpeed(orders_mode) == (*it2)->GetSpeed(orders_mode))
            {
                result = units1_first ? *it1 : *it2;
            }
            else if (part1 || Settings::Get().ExtBattleReverseWaitOrder())
            {
                if ((*it1)->GetSpeed(orders_mode) > (*it2)->GetSpeed(orders_mode))
                    result = *it1;
                else if ((*it2)->GetSpeed(orders_mode) > (*it1)->GetSpeed(orders_mode))
                    result = *it2;
            }
            else
            {
                if ((*it1)->GetSpeed(orders_mode) < (*it2)->GetSpeed(orders_mode))
                    result = *it1;
                else if ((*it2)->GetSpeed(orders_mode) < (*it1)->GetSpeed(orders_mode))
                    result = *it2;
            }
        }
        else if (it1 != units1._items.end())
            result = *it1;
        else if (it2 != units2._items.end())
            result = *it2;

        if (result && orders_mode)
        {
            if (it1 != units1._items.end() && result == *it1)
                units1._items.erase(it1);
            else if (it2 != units2._items.end() && result == *it2)
                units2._items.erase(it2);
        }

        return result;
    }
}

Battle::Force::Force(Army& parent, bool opposite) : army(parent)
{
    uids.reserve(army.m_troops.Size());

    for (uint32_t index = 0; index < army.m_troops.Size(); ++index)
    {
        const Troop* troop = army.m_troops.GetTroop(index);
        const uint32_t position = army.isSpreadFormat() ? index * 22 : 22 + index * 11;
        uint32_t uid = 0;

        if (troop && troop->IsValid())
        {
            _items.push_back(new Unit(*troop, opposite ? position + 10 : position, opposite));
            _items.back()->SetArmy(army);
            uid = _items.back()->GetUID();
        }

        uids.push_back(uid);
    }
}

Battle::Force::~Force()
{
    for (auto& it : _items) delete it;
}

const HeroBase* Battle::Force::GetCommander() const
{
    return army.GetCommander();
}

HeroBase* Battle::Force::GetCommander()
{
    return army.GetCommander();
}

int Battle::Force::GetColor() const
{
    return army.GetColor();
}

int Battle::Force::GetControl() const
{
    return army.GetControl();
}

bool Battle::Force::IsValid() const
{
    for (auto& it : _items)
    {
        if (it->isValid())
            return true;
    }
    return false;
}

uint32_t Battle::Force::GetSurrenderCost() const
{
    float res = 0;

    for (auto it : _items)
        if (it->isValid())
        {
            // FIXME: orig: 3 titan = 7500
            payment_t payment = it->GetCost();
            res += payment.gold;
        }

    const HeroBase* commander = GetCommander();

    if (commander)
    {
        switch (commander->GetLevelSkill(Skill::SkillT::DIPLOMACY))
        {
            // 40%
            case Skill::Level::BASIC:
                res = res * 40 / 100;
                break;
                // 30%
            case Skill::Level::ADVANCED:
                res = res * 30 / 100;
                break;
                // 20%
            case Skill::Level::EXPERT:
                res = res * 20 / 100;
                break;
                // 50%
            default:
                res = res * 50 / 100;
                break;
        }

        Artifact art(Artifact::STATESMAN_QUILL);

        if (commander->HasArtifact(art))
            res -= res * art.ExtraValue() / 100;
    }

    // limit
    if (res < 100) res = 100.0;

    return static_cast<uint32_t>(res);
}

void Battle::Force::NewTurn()
{
    if (GetCommander())
        GetCommander()->ResetModes(Heroes::SPELLCASTED);

    for_each(_items.begin(), _items.end(),
             [&](Unit* it) { it->NewTurn(); });
}

bool isUnitFirst(const Battle::Unit* last, bool part1, int army2_color)
{
    return (!last && part1) || (last && army2_color == last->GetColor());
}

void Battle::Force::UpdateOrderUnits(const Force& army1, const Force& army2, Units& orders)
{
    orders._items.clear();
    Unit* last = nullptr;

    Units units1(army1, true);
    Units units2(army2, true);
    units1.SortFastest(true);
    units2.SortFastest(true);
    while (nullptr !=
           (last = ForceGetCurrentUnitPart(units1, units2, true, isUnitFirst(last, true, army2.GetColor()), true)))
        orders._items.push_back(last);

    if (!Settings::Get().ExtBattleSoftWait())
        return;
    Units units11(army1, true);
    Units units21(army2, true);

    if (Settings::Get().ExtBattleReverseWaitOrder())
    {
        units11.SortFastest(true);
        units21.SortFastest(true);
    }
    else
    {
        units11.SortSlowest(true);
        units21.SortSlowest(true);
    }

    while (nullptr !=
           (last = ForceGetCurrentUnitPart(units11, units21, false, isUnitFirst(last, false, army2.GetColor()),
                                           true)))
        orders._items.push_back(last);
}

Battle::Unit* Battle::Force::GetCurrentUnit(const Force& army1, const Force& army2, Unit* last, bool part1)
{
    Units units1(army1, true);
    Units units2(army2, true);

    if (part1 || Settings::Get().ExtBattleReverseWaitOrder())
    {
        units1.SortFastest(false);
        units2.SortFastest(false);
    }
    else
    {
        units1.SortSlowest(false);
        units2.SortSlowest(false);
    }

    Unit* result = ForceGetCurrentUnitPart(units1, units2, part1, isUnitFirst(last, part1, army2.GetColor()), false);

    return result &&
           result->isValid() &&
           result->GetSpeed() > Speed::STANDING
           ? result
           : nullptr;
}

ByteVectorWriter& Battle::operator<<(ByteVectorWriter& msg, const Force& f)
{
    msg << static_cast<const BitModes &>(f) << static_cast<uint32_t>(f._items.size());

    for (auto it : f._items)
        msg << it->GetUID() << *it;

    return msg;
}


ByteVectorReader& Battle::operator>>(ByteVectorReader& msg, Force& f)
{
    uint32_t size = 0;
    uint32_t uid = 0;

    msg >> static_cast<BitModes &>(f) >> size;

    for (uint32_t ii = 0; ii < size; ++ii)
    {
        msg >> uid;
        Unit* b = f.FindUID(uid);
        if (b) msg >> *b;
    }

    return msg;
}

Troops Battle::Force::GetKilledTroops() const
{
    Troops killed;

    for (auto it : _items)
    {
        const Unit& b = *it;
        killed.PushBack(b._monster, b.GetDead());
    }

    return killed;
}

bool Battle::Force::SetIdleAnimation()
{
    bool res = false;

    for (auto& it : _items)
    {
        Unit& unit = *it;

        if (unit.isValid())
        {
            if (unit.isFinishAnimFrame())
                unit.ResetAnimFrame(AS_IDLE);
            else if (unit.isStartAnimFrame() && 3 > Rand::Get(1, 10))
            {
                unit.IncreaseAnimFrame();
                res = true;
            }
        }
    }

    return res;
}

bool Battle::Force::NextIdleAnimation()
{
    bool res = false;

    for (auto& it : _items)
    {
        Unit& unit = *it;

        if (unit.isValid() && !unit.isStartAnimFrame())
        {
            unit.IncreaseAnimFrame(false);
            res = true;
        }
    }

    return res;
}

bool Battle::Force::HasMonster(const Monster& mons) const
{
    return _items.end() != find_if(_items.begin(), _items.end(),
                                   [&](const Unit* it)
                                   {
                                       return it->isMonster(mons());
                                   });
}

uint32_t Battle::Force::GetDeadCounts() const
{
    uint32_t res = 0;

    for (auto it : _items)
        res += it->GetDead();

    return res;
}

uint32_t Battle::Force::GetDeadHitPoints() const
{
    uint32_t res = 0;

    for (auto it : _items)
    {
        res += Monster::GetHitPoints(it->_monster) * it->GetDead();
    }

    return res;
}

void Battle::Force::SyncArmyCount()
{
    for (uint32_t index = 0; index < army.m_troops.Size(); ++index)
    {
        Troop* troop = army.m_troops.GetTroop(index);

        if (!troop || !troop->IsValid())
            continue;
        const Unit* unit = FindUID(uids.at(index));

        if (unit && unit->GetDead())
            troop->SetCount(unit->GetDead() > troop->GetCount() ? 0 : troop->GetCount() - unit->GetDead());
    }
}

