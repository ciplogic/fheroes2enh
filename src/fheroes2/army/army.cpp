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
#include <functional>

#include "agg.h"
#include "settings.h"
#include "payment.h"
#include "world.h"
#include "text.h"
#include "race.h"
#include "game.h"
#include "luck.h"
#include "morale.h"

enum armysize_t
{
    ARMY_FEW = 1,
    ARMY_SEVERAL = 5,
    ARMY_PACK = 10,
    ARMY_LOTS = 20,
    ARMY_HORDE = 50,
    ARMY_THRONG = 100,
    ARMY_SWARM = 250,
    ARMY_ZOUNDS = 500,
    ARMY_LEGION = 1000
};

armysize_t ArmyGetSize(u32 count)
{
    if (ARMY_LEGION <= count) return ARMY_LEGION;
    else if (ARMY_ZOUNDS <= count) return ARMY_ZOUNDS;
    else if (ARMY_SWARM <= count) return ARMY_SWARM;
    else if (ARMY_THRONG <= count) return ARMY_THRONG;
    else if (ARMY_HORDE <= count) return ARMY_HORDE;
    else if (ARMY_LOTS <= count) return ARMY_LOTS;
    else if (ARMY_PACK <= count) return ARMY_PACK;
    else if (ARMY_SEVERAL <= count) return ARMY_SEVERAL;

    return ARMY_FEW;
}

string Army::TroopSizeString(const Troop &troop)
{
    string str;

    switch (ArmyGetSize(troop.GetCount()))
    {
        default:
            str = _("A few\n%{monster}");
            break;
        case ARMY_SEVERAL:
            str = _("Several\n%{monster}");
            break;
        case ARMY_PACK:
            str = _("A pack of\n%{monster}");
            break;
        case ARMY_LOTS:
            str = _("Lots of\n%{monster}");
            break;
        case ARMY_HORDE:
            str = _("A horde of\n%{monster}");
            break;
        case ARMY_THRONG:
            str = _("A throng of\n%{monster}");
            break;
        case ARMY_SWARM:
            str = _("A swarm of\n%{monster}");
            break;
        case ARMY_ZOUNDS:
            str = _("Zounds of\n%{monster}");
            break;
        case ARMY_LEGION:
            str = _("A legion of\n%{monster}");
            break;
    }

    StringReplace(str, "%{monster}", StringLower(troop.GetMultiName()));
    return str;
}

string Army::SizeString(u32 size)
{
    const char *str_size[] = {_("army|Few"), _("army|Several"), _("army|Pack"), _("army|Lots"),
                              _("army|Horde"), _("army|Throng"), _("army|Swarm"), _("army|Zounds"), _("army|Legion")};

    switch (ArmyGetSize(size))
    {
        default:
            break;
        case ARMY_SEVERAL:
            return str_size[1];
        case ARMY_PACK:
            return str_size[2];
        case ARMY_LOTS:
            return str_size[3];
        case ARMY_HORDE:
            return str_size[4];
        case ARMY_THRONG:
            return str_size[5];
        case ARMY_SWARM:
            return str_size[6];
        case ARMY_ZOUNDS:
            return str_size[7];
        case ARMY_LEGION:
            return str_size[8];
    }

    return str_size[0];
}

Troops::Troops()
= default;

Troops::~Troops()
{
	for (auto it : _items) {
#ifndef WIN32
		delete it;
#endif
		it = nullptr;
	}
	_items.clear();
}

size_t Troops::Size() const
{
    return _items.size();
}

void Troops::Assign(const Troop *it1, const Troop *it2)
{
    Clean();

    auto it = _items.begin();

    while (it != _items.end() && it1 != it2)
    {
        if (it1->isValid()) (*it)->Set(*it1);
        ++it;
        ++it1;
    }
}

void Troops::Assign(const Troops &troops)
{
    Clean();

    auto it1 = _items.begin();
    auto it2 = troops._items.begin();

    while (it1 != _items.end() && it2 != troops._items.end())
    {
        if ((*it2)->isValid()) (*it1)->Set(**it2);
        ++it2;
        ++it1;
    }
}

void Troops::Insert(const Troops &troops)
{
    for (auto it: troops._items)
        _items.push_back(new Troop(*it));
}

void Troops::PushBack(const Monster &mons, u32 count)
{
    _items.push_back(new Troop(mons, count));
}

void Troops::PopBack()
{
    if (!this->_items.empty())
    {
        delete _items.back();
        _items.pop_back();
    }
}

Troop *Troops::GetTroop(size_t pos)
{
    return pos < _items.size() ? _items.at(pos) : nullptr;
}

const Troop *Troops::GetTroop(size_t pos) const
{
    return pos < _items.size() ? _items.at(pos) : nullptr;
}

void Troops::UpgradeMonsters(const Monster &m)
{
    for (auto it: _items)
        if ((it)->isValid() && *it == m) (it)->Upgrade();
}

u32 Troops::GetCountMonsters(const Monster &m) const
{
    u32 c = 0;

    for (const auto it : _items)
        if ((it)->isValid() && *it == m) c += (it)->GetCount();

    return c;
}

bool Troops::isValid() const
{
    return _items.end() != find_if(_items.begin(), _items.end(), mem_fun(&Troop::isValid));
}

u32 Troops::GetCount() const
{
    return count_if(_items.begin(), _items.end(), mem_fun(&Troop::isValid));
}

bool Troops::HasMonster(const Monster &mons) const
{
    return _items.end() != find_if(_items.begin(), _items.end(), bind2nd(mem_fun(&Troop::isMonster), mons()));
}

bool Troops::AllTroopsIsRace(int race) const
{
    for (const auto it: _items)
        if ((it)->isValid() && (it)->GetRace() != race) return false;

    return true;
}

bool Troops::CanJoinTroop(const Monster &mons) const
{
    auto it = find_if(_items.begin(), _items.end(), bind2nd(mem_fun(&Troop::isMonster), mons()));
    if (it == _items.end()) it = find_if(_items.begin(), _items.end(), not1(mem_fun(&Troop::isValid)));

    return it != _items.end();
}

bool Troops::JoinTroop(const Monster &mons, u32 count)
{
    if (mons.isValid() && count)
    {
        auto it = find_if(_items.begin(), _items.end(), bind2nd(mem_fun(&Troop::isMonster), mons()));
        if (it == _items.end()) it = find_if(_items.begin(), _items.end(), not1(mem_fun(&Troop::isValid)));

        if (it != _items.end())
        {
            if ((*it)->isValid())
                (*it)->SetCount((*it)->GetCount() + count);
            else
                (*it)->Set(mons, count);

            DEBUG(DBG_GAME, DBG_INFO, std::dec << count << " " << (*it)->GetName());
            return true;
        }
    }

    return false;
}

bool Troops::JoinTroop(const Troop &troop)
{
    return troop.isValid() && JoinTroop(troop(), troop.GetCount());
}

bool Troops::CanJoinTroops(const Troops &troops2) const
{
    if (this == &troops2)
        return false;

    Army troops1;
    troops1.Insert(*this);

    for (auto it : troops2._items)
        if ((it)->isValid() && !troops1.JoinTroop(*it)) return false;

    return true;
}

void Troops::JoinTroops(Troops &troops2)
{
    if (this == &troops2)
        return;

    for (auto &_item : troops2._items)
        if (_item->isValid())
        {
            JoinTroop(*_item);
            _item->Reset();
        }
}

u32 Troops::GetUniqueCount() const
{
    vector<int> monsters;
    monsters.reserve(_items.size());

    for (auto _item : _items)
        if (_item->isValid()) monsters.push_back(_item->GetID());

    sort(monsters.begin(), monsters.end());
    monsters.resize(distance(monsters.begin(),
                                  unique(monsters.begin(), monsters.end())));

    return monsters.size();
}


u32 Troops::GetAttack() const
{
    u32 res = 0;
    u32 count = 0;

    for (auto _item : _items)
        if (_item->isValid())
        {
            res += static_cast<Monster *>(_item)->GetAttack();
            ++count;
        }

    return count ? res / count : 0;
}

u32 Troops::GetDefense() const
{
    u32 res = 0;
    u32 count = 0;

    for (auto _item : _items)
        if (_item->isValid())
        {
            res += static_cast<Monster *>(_item)->GetDefense();
            ++count;
        }

    return count ? res / count : 0;
}

u32 Troops::GetHitPoints() const
{
    u32 res = 0;

    for (auto _item : _items)
        if (_item->isValid()) res += _item->GetHitPoints();

    return res;
}

u32 Troops::GetDamageMin() const
{
    u32 res = 0;
    u32 count = 0;

    for (auto _item : _items)
        if (_item->isValid())
        {
            res += _item->GetDamageMin();
            ++count;
        }

    return count ? res / count : 0;
}

u32 Troops::GetDamageMax() const
{
    u32 res = 0;
    u32 count = 0;

    for (auto _item : _items)
        if (_item->isValid())
        {
            res += _item->GetDamageMax();
            ++count;
        }

    return count ? res / count : 0;
}

u32 Troops::GetStrength() const
{
    u32 res = 0;

    for (auto _item : _items)
        if (_item->isValid()) res += _item->GetStrength();

    return res;
}

void Troops::Clean()
{
    for_each(_items.begin(), _items.end(), mem_fun(&Troop::Reset));
}

void Troops::UpgradeTroops(const Castle &castle)
{
    for (auto &_item : _items)
        if (_item->isValid())
        {
            payment_t payment = _item->GetUpgradeCost();
            Kingdom &kingdom = castle.GetKingdom();

            if (castle.GetRace() == _item->GetRace() &&
                castle.isBuild(_item->GetUpgrade().GetDwelling()) &&
                kingdom.AllowPayment(payment))
            {
                kingdom.OddFundsResource(payment);
                _item->Upgrade();
            }
        }
}

Troop *Troops::GetFirstValid()
{
    auto it = find_if(_items.begin(), _items.end(), mem_fun(&Troop::isValid));
    return it == _items.end() ? nullptr : *it;
}

Troop *Troops::GetWeakestTroop()
{
    vector<Troop*>::iterator first, last, lowest;

    first = _items.begin();
    last = _items.end();

    while (first != last) if ((*first)->isValid()) break; else ++first;

    if (first == _items.end()) return nullptr;
    lowest = first;

    if (first != last)
        while (++first != last) if ((*first)->isValid() && Army::WeakestTroop(*first, *lowest)) lowest = first;

    return *lowest;
}

Troop *Troops::GetSlowestTroop()
{
    vector<Troop*>::iterator first, last, lowest;

    first = _items.begin();
    last = _items.end();

    while (first != last) if ((*first)->isValid()) break; else ++first;

    if (first == _items.end()) return nullptr;
    lowest = first;

    if (first != last)
        while (++first != last) if ((*first)->isValid() && Army::SlowestTroop(*first, *lowest)) lowest = first;

    return *lowest;
}

Troops Troops::GetOptimized() const
{
    Troops result;
    result._items.reserve(_items.size());

    for (auto _item : _items)
        if (_item->isValid())
        {
            auto it2 = find_if(result._items.begin(), result._items.end(),
                                        bind2nd(mem_fun(&Troop::isMonster), _item->GetID()));

            if (it2 == result._items.end())
                result._items.push_back(new Troop(*_item));
            else
                (*it2)->SetCount((*it2)->GetCount() + _item->GetCount());
        }

    return result;
}

void Troops::ArrangeForBattle(bool upgrade)
{
    Troops priority = GetOptimized();

    switch (priority._items.size())
    {
        case 1:
        {
	        Troop* mPtr = priority._items.back();
            const Monster &m = *mPtr;
            const u32 count = mPtr->GetCount();

            Clean();

            if (49 < count)
            {
                const u32 c = count / 5;
                _items.at(0)->Set(m, c);
                _items.at(1)->Set(m, c);
                _items.at(2)->Set(m, c + count - (c * 5));
                _items.at(3)->Set(m, c);
                _items.at(4)->Set(m, c);

                if (upgrade && _items.at(2)->isAllowUpgrade())
                    _items.at(2)->Upgrade();
            } else if (20 < count)
            {
                const u32 c = count / 3;
                _items.at(1)->Set(m, c);
                _items.at(2)->Set(m, c + count - (c * 3));
                _items.at(3)->Set(m, c);

                if (upgrade && _items.at(2)->isAllowUpgrade())
                    _items.at(2)->Upgrade();
            } else
                _items.at(2)->Set(m, count);
            break;
        }
        case 2:
        {
            // TODO: need modify army for 2 troops
            Assign(priority);
            break;
        }
        case 3:
        {
            // TODO: need modify army for 3 troops
            Assign(priority);
            break;
        }
        case 4:
        {
            // TODO: need modify army for 4 troops
            Assign(priority);
            break;
        }
        case 5:
        {
            // possible change orders monster
            // store
            Assign(priority);
            break;
        }
        default:
            break;
    }
}

void Troops::JoinStrongest(Troops &troops2, bool save_last)
{
    if (this == &troops2)
        return;

    Troops priority = GetOptimized();
    priority._items.reserve(ARMYMAXTROOPS * 2);

    Troops priority2 = troops2.GetOptimized();
    priority.Insert(priority2);

    Clean();
    troops2.Clean();

    // sort: strongest
    sort(priority._items.begin(), priority._items.end(), Army::StrongestTroop);

    // weakest to army2
    while (_items.size() < priority._items.size())
    {
        troops2.JoinTroop(*priority._items.back());
        priority.PopBack();
    }

    // save half weak of strongest to army2
    if (save_last && !troops2.isValid())
    {
        Troop &last = *priority._items.back();
        u32 count = last.GetCount() / 2;
        troops2.JoinTroop(last, last.GetCount() - count);
        last.SetCount(count);
    }

    // strongest to army
    while (!priority._items.empty())
    {
        JoinTroop(*priority._items.back());
        priority.PopBack();
    }
}

void Troops::KeepOnlyWeakest(Troops &troops2, bool save_last)
{
    if (this == &troops2)
        return;

    Troops priority = GetOptimized();
    priority._items.reserve(ARMYMAXTROOPS * 2);

    Troops priority2 = troops2.GetOptimized();
    priority.Insert(priority2);

    Clean();
    troops2.Clean();

    // sort: strongest
    sort(priority._items.begin(), priority._items.end(), Army::StrongestTroop);

    // weakest to army
    while (_items.size() < priority._items.size())
    {
        JoinTroop(*priority._items.back());
        priority.PopBack();
    }

    // save half weak of strongest to army
    if (save_last && !isValid())
    {
        Troop &last = *priority._items.back();
        u32 count = last.GetCount() / 2;
        JoinTroop(last, last.GetCount() - count);
        last.SetCount(count);
    }

    // strongest to army2
    while (!priority._items.empty())
    {
        troops2.JoinTroop(*priority._items.back());
        priority.PopBack();
    }
}

void Troops::DrawMons32LineWithScoute(s32 cx, s32 cy, u32 width, u32 first, u32 count, u32 scoute, bool shorts) const
{
    if (isValid())
    {
        if (0 == count) count = GetCount();

        const u32 chunk = width / count;
        cx += chunk / 2;

        Text text;
        text.Set(Font::SMALL);

        for (auto _item : _items)
            if (_item->isValid())
            {
                if (0 == first && count)
                {
                    const Sprite &monster = AGG::GetICN(ICN::MONS32, _item->GetSpriteIndex());

                    monster.Blit(cx - monster.w() / 2, cy + 30 - monster.h());
                    text.Set(Game::CountScoute(_item->GetCount(), scoute, shorts));
                    text.Blit(cx - text.w() / 2, cy + 28);

                    cx += chunk;
                    --count;
                } else
                    --first;
            }
    }
}

void Troops::SplitTroopIntoFreeSlots(const Troop &troop, u32 slots)
{
    if (slots && slots <= (Size() - GetCount()))
    {
        u32 chunk = troop.GetCount() / slots;
        u32 limits = slots;
        vector<vector<Troop*>::iterator> iters;

        for (auto it = _items.begin(); it != _items.end(); ++it)
            if (!(*it)->isValid() && limits)
            {
                iters.push_back(it);
                (*it)->Set(troop.GetMonster(), chunk);
                --limits;
            }

        u32 last = troop.GetCount() - chunk * slots;

        for (auto &iter : iters)
        {
            if (last)
            {
                (*iter)->SetCount((*iter)->GetCount() + 1);
                --last;
            }
        }
    }
}


Army::Army(HeroBase *s) : commander(s), combat_format(true), color(Color::NONE)
{
    _items.reserve(ARMYMAXTROOPS);
    for (u32 ii = 0; ii < ARMYMAXTROOPS; ++ii) _items.push_back(new ArmyTroop(this));
}

Army::Army(const Maps::Tiles &t) : commander(nullptr), combat_format(true), color(Color::NONE)
{
    _items.reserve(ARMYMAXTROOPS);
    for (u32 ii = 0; ii < ARMYMAXTROOPS; ++ii) _items.push_back(new ArmyTroop(this));

    if (MP2::isCaptureObject(t.GetObject()))
        color = t.QuantityColor();

    switch (t.GetObject(false))
    {
        case MP2::OBJ_PYRAMID:
            _items.at(0)->Set(Monster::ROYAL_MUMMY, 10);
            _items.at(1)->Set(Monster::VAMPIRE_LORD, 10);
            _items.at(2)->Set(Monster::ROYAL_MUMMY, 10);
            _items.at(3)->Set(Monster::VAMPIRE_LORD, 10);
            _items.at(4)->Set(Monster::ROYAL_MUMMY, 10);
            break;

        case MP2::OBJ_GRAVEYARD:
            _items.at(0)->Set(Monster::MUTANT_ZOMBIE, 100);
            ArrangeForBattle(false);
            break;

        case MP2::OBJ_SHIPWRECK:
            _items.at(0)->Set(Monster::GHOST, t.GetQuantity2());
            ArrangeForBattle(false);
            break;

        case MP2::OBJ_DERELICTSHIP:
            _items.at(0)->Set(Monster::SKELETON, 200);
            ArrangeForBattle(false);
            break;

        case MP2::OBJ_ARTIFACT:
            switch (t.QuantityVariant())
            {
                case 6:
                    _items.at(0)->Set(Monster::ROGUE, 50);
                    break;
                case 7:
                    _items.at(0)->Set(Monster::GENIE, 1);
                    break;
                case 8:
                    _items.at(0)->Set(Monster::PALADIN, 1);
                    break;
                case 9:
                    _items.at(0)->Set(Monster::CYCLOPS, 1);
                    break;
                case 10:
                    _items.at(0)->Set(Monster::PHOENIX, 1);
                    break;
                case 11:
                    _items.at(0)->Set(Monster::GREEN_DRAGON, 1);
                    break;
                case 12:
                    _items.at(0)->Set(Monster::TITAN, 1);
                    break;
                case 13:
                    _items.at(0)->Set(Monster::BONE_DRAGON, 1);
                    break;
                default:
                    break;
            }
            ArrangeForBattle(false);
            break;

            //case MP2::OBJ_ABANDONEDMINE:
            //    at(0) = Troop(t);
            //    ArrangeForBattle(false);
            //    break;

        case MP2::OBJ_CITYDEAD:
            _items.at(0)->Set(Monster::ZOMBIE, 20);
           _items.at(1)->Set(Monster::VAMPIRE_LORD, 5);
           _items.at(2)->Set(Monster::POWER_LICH, 5);
           _items.at(3)->Set(Monster::VAMPIRE_LORD, 5);
           _items.at(4)->Set(Monster::ZOMBIE, 20);
            break;

        case MP2::OBJ_TROLLBRIDGE:
            _items.at(0)->Set(Monster::TROLL, 4);
           _items.at(1)->Set(Monster::WAR_TROLL, 4);
           _items.at(2)->Set(Monster::TROLL, 4);
           _items.at(3)->Set(Monster::WAR_TROLL, 4);
           _items.at(4)->Set(Monster::TROLL, 4);
            break;

        case MP2::OBJ_DRAGONCITY:
            _items.at(0)->Set(Monster::GREEN_DRAGON, 3);
           _items.at(1)->Set(Monster::RED_DRAGON, 2);
           _items.at(2)->Set(Monster::BLACK_DRAGON, 1);
            break;

        case MP2::OBJ_DAEMONCAVE:
            _items.at(0)->Set(Monster::EARTH_ELEMENT, 2);
           _items.at(1)->Set(Monster::EARTH_ELEMENT, 2);
           _items.at(2)->Set(Monster::EARTH_ELEMENT, 2);
           _items.at(3)->Set(Monster::EARTH_ELEMENT, 2);
            break;

        default:
            if (MP2::isCaptureObject(t.GetObject()))
            {
                CapturedObject &co = world.GetCapturedObject(t.GetIndex());
                Troop &troop = co.GetTroop();

                switch (co.GetSplit())
                {
                    case 3:
                        if (3 > troop.GetCount())
                            _items.at(0)->Set(co.GetTroop());
                        else
                        {
                            _items.at(0)->Set(troop(), troop.GetCount() / 3);
                           _items.at(4)->Set(troop(), troop.GetCount() / 3);
                           _items.at(2)->Set(troop(), troop.GetCount() - _items.at(4)->GetCount() - _items.at(0)->GetCount());
                        }
                        break;

                    case 5:
                        if (5 > troop.GetCount())
                            _items.at(0)->Set(co.GetTroop());
                        else
                        {
                            _items.at(0)->Set(troop(), troop.GetCount() / 5);
                           _items.at(1)->Set(troop(), troop.GetCount() / 5);
                           _items.at(3)->Set(troop(), troop.GetCount() / 5);
                           _items.at(4)->Set(troop(), troop.GetCount() / 5);
                           _items.at(2)->Set(troop(),
                                       troop.GetCount() - _items.at(0)->GetCount() - _items.at(1)->GetCount() - _items.at(3)->GetCount() -
                                      _items.at(4)->GetCount());
                        }
                        break;

                    default:
                        _items.at(0)->Set(co.GetTroop());
                        break;
                }
            } else
            {
                auto *map_troop = static_cast<MapMonster *>(world.GetMapObject(t.GetObjectUID(MP2::OBJ_MONSTER)));
                Troop troop = map_troop ? map_troop->QuantityTroop() : t.QuantityTroop();

                _items.at(0)->Set(troop);
                ArrangeForBattle(!Settings::Get().ExtWorldSaveMonsterBattle());
            }
            break;
    }
}

Army::~Army()
{
	for (auto &_item : _items)
    {
		delete _item;
        _item = nullptr;
	}
    _items.clear();
}

bool Army::isFullHouse() const
{
    return GetCount() == _items.size();
}

void Army::SetSpreadFormat(bool f)
{
    combat_format = f;
}

bool Army::isSpreadFormat() const
{
    return combat_format;
}

int Army::GetColor() const
{
    return GetCommander() ? GetCommander()->GetColor() : color;
}

void Army::SetColor(int cl)
{
    color = cl;
}

int Army::GetRace() const
{
    vector<int> races;
    races.reserve(_items.size());

    for (auto _item : _items)
        if (_item->isValid()) races.push_back(_item->GetRace());

    sort(races.begin(), races.end());
    races.resize(distance(races.begin(), unique(races.begin(), races.end())));

    if (races.empty())
    {
        DEBUG(DBG_GAME, DBG_WARN, "empty");
        return Race::NONE;
    }

    return 1 < races.size() ? Race::MULT : races[0];
}

int Army::GetLuck() const
{
    return GetCommander() ? GetCommander()->GetLuck() : GetLuckModificator(nullptr);
}

int Army::GetLuckModificator(string *strs) const
{
    return Luck::NORMAL;
}

int Army::GetMorale() const
{
    return GetCommander() ? GetCommander()->GetMorale() : GetMoraleModificator(nullptr);
}

// TODO:: need optimize
int Army::GetMoraleModificator(string *strs) const
{
    int result = Morale::NORMAL;

    // different race penalty
    u32 count = 0;
    u32 count_kngt = 0;
    u32 count_barb = 0;
    u32 count_sorc = 0;
    u32 count_wrlk = 0;
    u32 count_wzrd = 0;
    u32 count_necr = 0;
    u32 count_bomg = 0;
    bool ghost_present = false;

    for (auto _item : _items)
        if (_item->isValid())
        {
            switch (_item->GetRace())
            {
                case Race::KNGT:
                    ++count_kngt;
                    break;
                case Race::BARB:
                    ++count_barb;
                    break;
                case Race::SORC:
                    ++count_sorc;
                    break;
                case Race::WRLK:
                    ++count_wrlk;
                    break;
                case Race::WZRD:
                    ++count_wzrd;
                    break;
                case Race::NECR:
                    ++count_necr;
                    break;
                case Race::NONE:
                    ++count_bomg;
                    break;
                default:
                    break;
            }
            if (_item->GetID() == Monster::GHOST) ghost_present = true;
        }

    u32 r = Race::MULT;
    if (count_kngt)
    {
        ++count;
        r = Race::KNGT;
    }
    if (count_barb)
    {
        ++count;
        r = Race::BARB;
    }
    if (count_sorc)
    {
        ++count;
        r = Race::SORC;
    }
    if (count_wrlk)
    {
        ++count;
        r = Race::WRLK;
    }
    if (count_wzrd)
    {
        ++count;
        r = Race::WZRD;
    }
    if (count_necr)
    {
        ++count;
        r = Race::NECR;
    }
    if (count_bomg)
    {
        ++count;
        r = Race::NONE;
    }
    const u32 uniq_count = GetUniqueCount();

    switch (count)
    {
        case 2:
        case 0:
            break;
        case 1:
            if (0 == count_necr && !ghost_present)
            {
                if (1 < uniq_count)
                {
                    ++result;
                    if (strs)
                    {
                        string str = _("All %{race} troops +1");
                        StringReplace(str, "%{race}", Race::String(r));
                        strs->append(str);
                        strs->append("\n");
                    }
                }
            } else
            {
                if (strs)
                {
                    strs->append(_("Entire unit is undead, so morale does not apply."));
                    strs->append("\n");
                }
                return 0;
            }
            break;
        case 3:
            result -= 1;
            if (strs)
            {
                strs->append(_("Troops of 3 alignments -1"));
                strs->append("\n");
            }
            break;
        case 4:
            result -= 2;
            if (strs)
            {
                strs->append(_("Troops of 4 alignments -2"));
                strs->append("\n");
            }
            break;
        default:
            result -= 3;
            if (strs)
            {
                strs->append(_("Troops of 5 alignments -3"));
                strs->append("\n");
            }
            break;
    }

    // undead in life group
    if ((1 < uniq_count && (count_necr || ghost_present) &&
         (count_kngt || count_barb || count_sorc || count_wrlk || count_wzrd || count_bomg)) ||
        // or artifact Arm Martyr
        (GetCommander() && GetCommander()->HasArtifact(Artifact::ARM_MARTYR)))
    {
        result -= 1;
        if (strs)
        {
            strs->append(_("Some undead in groups -1"));
            strs->append("\n");
        }
    }

    return result;
}

u32 Army::GetAttack() const
{
    u32 res = 0;
    u32 count = 0;

    for (auto _item : _items)
        if (_item->isValid())
        {
            res += _item->GetAttack();
            ++count;
        }

    return count ? res / count : 0;
}

u32 Army::GetDefense() const
{
    u32 res = 0;
    u32 count = 0;

    for (auto _item : _items)
        if (_item->isValid())
        {
            res += _item->GetDefense();
            ++count;
        }

    return count ? res / count : 0;
}

void Army::Reset(bool soft)
{
    Clean();

    if (commander && commander->isHeroes())
    {
        const Monster mons1(commander->GetRace(), DWELLING_MONSTER1);

        if (soft)
        {
            const Monster mons2(commander->GetRace(), DWELLING_MONSTER2);

            switch (Rand::Get(1, 3))
            {
                case 1:
                    JoinTroop(mons1, 3 * mons1.GetGrown());
                    break;
                case 2:
                    JoinTroop(mons2, mons2.GetGrown() + mons2.GetGrown() / 2);
                    break;
                default:
                    JoinTroop(mons1, 2 * mons1.GetGrown());
                    JoinTroop(mons2, mons2.GetGrown());
                    break;
            }
        } else
        {
            JoinTroop(mons1, 1);
        }
    }
}

void Army::SetCommander(HeroBase *c)
{
    commander = c;
}

HeroBase *Army::GetCommander()
{
    return (!commander || (commander->isCaptain() && !commander->isValid())) ? nullptr : commander;
}

const Castle *Army::inCastle() const
{
    return commander ? commander->inCastle() : nullptr;
}

const HeroBase *Army::GetCommander() const
{
    return (!commander || (commander->isCaptain() && !commander->isValid())) ? nullptr : commander;
}

int Army::GetControl() const
{
    return commander ? commander->GetControl() : (color == Color::NONE ? CONTROL_AI : Players::GetPlayerControl(color));
}

string Army::String() const
{
    ostringstream os;

    os << "color(" << Color::String(commander ? commander->GetColor() : color) << "), ";

    if (GetCommander())
        os << "commander(" << GetCommander()->GetName() << "), ";

    os << ": ";

    for (auto _item : _items)
        if (_item->isValid())
            os << dec << _item->GetCount() << " " << _item->GetName() << ", ";

    return os.str();
}

void Army::JoinStrongestFromArmy(Army &army2)
{
    bool save_last = army2.commander && army2.commander->isHeroes();
    JoinStrongest(army2, save_last);
}

void Army::KeepOnlyWeakestTroops(Army &army2)
{

    bool save_last = commander && commander->isHeroes();
    KeepOnlyWeakest(army2, save_last);
}

u32 Army::ActionToSirens()
{
    u32 res = 0;

    for (auto &_item : _items)
        if (_item->isValid())
        {
            const u32 kill = _item->GetCount() * 30 / 100;

            if (kill)
            {
                _item->SetCount(_item->GetCount() - kill);
                res += kill * static_cast<Monster *>(_item)->GetHitPoints();
            }
        }

    return res;
}

bool Army::TroopsStrongerEnemyTroops(const Troops &troops1, const Troops &troops2)
{
    if (!troops2.isValid()) return true;

    const int a1 = troops1.GetAttack();
    const int d1 = troops1.GetDefense();
    float r1 = 0;

    const int a2 = troops2.GetAttack();
    const int d2 = troops2.GetDefense();
    float r2 = 0;

    if (a1 > d2)
        r1 = 1 + 0.1 * static_cast<float>(min(a1 - d2, 20));
    else
        r1 = 1 + 0.05 * static_cast<float>(min(d2 - a1, 14));

    if (a2 > d1)
        r2 = 1 + 0.1 * static_cast<float>(min(a2 - d1, 20));
    else
        r2 = 1 + 0.05 * static_cast<float>(min(d1 - a2, 14));

    const u32 s1 = troops1.GetStrength();
    const u32 s2 = troops2.GetStrength();

    const float h1 = troops1.GetHitPoints();
    const float h2 = troops2.GetHitPoints();

    DEBUG(DBG_AI, DBG_TRACE, "r1: " << r1 << ", s1: " << s1 << ", h1: " << h1 \
 << ", r2: " << r2 << ", s2: " << s2 << ", h2: " << h2);

    r1 *= s1 / h2;
    r2 *= s2 / h1;

    return static_cast<s32>(r1) > static_cast<s32>(r2);
}

void Army::DrawMons32LineWithScoute(const Troops &troops, s32 cx, s32 cy, u32 width, u32 first, u32 count, u32 scoute)
{
    troops.DrawMons32LineWithScoute(cx, cy, width, first, count, scoute, false);
}

/* draw MONS32 sprite in line, first valid = 0, count = 0 */
void Army::DrawMons32Line(const Troops &troops, s32 cx, s32 cy, u32 width, u32 first, u32 count)
{
    troops.DrawMons32LineWithScoute(cx, cy, width, first, count, Skill::Level::EXPERT, false);
}

void Army::DrawMons32LineShort(const Troops &troops, s32 cx, s32 cy, u32 width, u32 first, u32 count)
{
    troops.DrawMons32LineWithScoute(cx, cy, width, first, count, Skill::Level::EXPERT, true);
}

JoinCount Army::GetJoinSolution(const Heroes &hero, const Maps::Tiles &tile, const Troop &troop)
{
    MapMonster *map_troop = static_cast<MapMonster *>(world.GetMapObject(tile.GetObjectUID(MP2::OBJ_MONSTER)));
    const u32 ratios = troop.isValid() ? hero.GetArmy().GetHitPoints() / troop.GetHitPoints() : 0;
    const bool check_free_stack = true; // (hero.GetArmy().GetCount() < hero.GetArmy().size() || hero.GetArmy().HasMonster(troop)); // set force, see Dialog::ArmyJoinWithCost, http://sourceforge.net/tracker/?func=detail&aid=3567985&group_id=96859&atid=616183
    const bool check_extra_condition = (!hero.HasArtifact(Artifact::HIDEOUS_MASK) &&
                                        Morale::NORMAL <= hero.GetMorale());

    const bool join_skip = map_troop ? map_troop->JoinConditionSkip() : tile.MonsterJoinConditionSkip();
    const bool join_free = map_troop ? map_troop->JoinConditionFree() : tile.MonsterJoinConditionFree();
    // force join for campain and others...
    const bool join_force = map_troop ? map_troop->JoinConditionForce() : tile.MonsterJoinConditionForce();

    if (!join_skip &&
        check_free_stack && ((check_extra_condition && ratios >= 2) || join_force))
    {
        if (join_free || join_force)
            return JoinCount(JOIN_FREE, troop.GetCount());
        else if (hero.HasSecondarySkill(Skill::Secondary::DIPLOMACY))
        {
            // skill diplomacy
            const u32 to_join = Monster::GetCountFromHitPoints(troop,
                                                               troop.GetHitPoints() *
                                                               hero.GetSecondaryValues(Skill::Secondary::DIPLOMACY) /
                                                               100);

            if (to_join)
                return JoinCount(JOIN_COST, to_join);
        }
    } else if (ratios >= 5)
    {
        // ... surely flee before us
        if (!hero.isControlAI() ||
            Rand::Get(0, 10) < 5)
            return JoinCount(JOIN_FLEE, 0);
    }

    return JoinCount(JOIN_NONE, 0);
}

bool Army::WeakestTroop(const Troop *t1, const Troop *t2)
{
    return t1->GetDamageMax() < t2->GetDamageMax();
}

bool Army::StrongestTroop(const Troop *t1, const Troop *t2)
{
    return t1->GetDamageMin() > t2->GetDamageMin();
}

bool Army::SlowestTroop(const Troop *t1, const Troop *t2)
{
    return t1->GetSpeed() < t2->GetSpeed();
}

bool Army::FastestTroop(const Troop *t1, const Troop *t2)
{
    return t1->GetSpeed() > t2->GetSpeed();
}

void Army::SwapTroops(Troop &t1, Troop &t2)
{
    swap(t1, t2);
}

bool Army::SaveLastTroop() const
{
    return commander && commander->isHeroes() && 1 == GetCount();
}

StreamBase &operator<<(StreamBase &msg, const Army &army)
{
    msg << static_cast<u32>(army._items.size());

    // Army: fixed size
    for (auto _item : army._items)
        msg << *_item;

    return msg << army.combat_format << army.color;
}

StreamBase &operator>>(StreamBase &msg, Army &army)
{
    u32 armysz;
    msg >> armysz;

    for (auto &_item : army._items)
        msg >> *_item;

    msg >> army.combat_format >> army.color;

    // set army
    for (auto it = army._items.begin(); it != army._items.end(); ++it)
    {
        auto *troop = dynamic_cast<ArmyTroop *>(*it);
        if (troop) troop->SetArmy(army);
    }

    // set later from owner (castle, heroes)
    army.commander = nullptr;

    return msg;
}

u32 Army::UniqueCount(const Army &a)
{
    return a.GetUniqueCount();
}
