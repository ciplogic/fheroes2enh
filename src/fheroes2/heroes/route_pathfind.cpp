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

#include "maps.h"
#include "ai.h"
#include "world.h"
#include "settings.h"
#include "ground.h"

struct cell_t
{
    cell_t() : cost_g(MAXU16), cost_t(MAXU16), cost_d(MAXU16), passbl(0), open(1), direct(Direction::CENTER), parent(-1)
    {
    }

    u16 cost_g; // ground
    u16 cost_t; // total
    u16 cost_d; // distance
    u16 passbl;
    u16 open; // bool
    u16 direct;
    s32 parent;
};

int GetCurrentLength(map<s32, cell_t>& list, s32 from)
{
    int res = 0;
    while (0 <= list[from].parent)
    {
        from = list[from].parent;
        ++res;
    }
    return res;
}

bool CheckMonsterProtectionAndNotDst(const s32& to, const s32& dst)
{
    const MapsIndexes& monsters = Maps::GetTilesUnderProtection(to);
    return !monsters.empty() && monsters.end() == find(monsters.begin(), monsters.end(), dst);
}

bool PassableToTile(const Heroes& hero, const Maps::Tiles& toTile, int direct, s32 dst)
{
    // check end point
    if (toTile.GetIndex() == dst)
    {
        // fix toTilePassable with action object
        if (MP2::isPickupObject(toTile.GetObject()))
            return true;

        // check direct to object
        if (MP2::isActionObject(toTile.GetObject(false), hero.isShipMaster()))
            return Direction::Reflect(direct) & toTile.GetPassable();

        if (MP2::OBJ_HEROES == toTile.GetObject())
            return toTile.isPassable(nullptr, Direction::Reflect(direct),
                                     hero.isControlAI() ? AI::HeroesSkipFog() : false);
    }

    // check to tile direct
    if (!toTile.isPassable(&hero, Direction::Reflect(direct), hero.isControlAI() ? AI::HeroesSkipFog() : false))
        return false;

    if (toTile.GetIndex() != dst)
    {
        if (MP2::isPickupObject(toTile.GetObject()) ||
            MP2::isActionObject(toTile.GetObject(false), hero.isShipMaster()))
            return false;

        // check hero/monster on route
        switch (toTile.GetObject())
        {
        case MP2::OBJ_HEROES:
        case MP2::OBJ_MONSTER:
            return false;

        default:
            break;
        }

        // check monster protection
        if (CheckMonsterProtectionAndNotDst(toTile.GetIndex(), dst))
            return false;
    }

    return true;
}

bool PassableFromToTile(const Heroes& hero, s32 from, const s32& to, int direct, s32 dst)
{
    const Maps::Tiles& fromTile = world.GetTiles(from);
    const Maps::Tiles& toTile = world.GetTiles(to);

    // check start point
    if (hero.GetIndex() == from)
    {
        if (MP2::isActionObject(fromTile.GetObject(false), hero.isShipMaster()))
        {
            // check direct from object
            if (!(direct & fromTile.GetPassable()))
                return false;
        }
        else
        {
            // check from tile direct
            if (!fromTile.isPassable(&hero, direct, hero.isControlAI() ? AI::HeroesSkipFog() : false))
                return false;
        }
    }
    else
    {
        if (MP2::isActionObject(fromTile.GetObject(), hero.isShipMaster()))
        {
            // check direct from object
            if (!(direct & fromTile.GetPassable()))
                return false;
        }
        else
        {
            // check from tile direct
            if (!fromTile.isPassable(&hero, direct, hero.isControlAI() ? AI::HeroesSkipFog() : false))
                return false;
        }
    }

    if (fromTile.isWater() && !toTile.isWater())
    {
        switch (toTile.GetObject())
        {
        case MP2::OBJ_BOAT:
        case MP2::OBJ_MONSTER:
        case MP2::OBJ_HEROES:
            return false;

        case MP2::OBJ_COAST:
            return toTile.GetIndex() == dst;

        default:
            break;
        }
    }
    else if (!fromTile.isWater() && toTile.isWater())
    {
        switch (toTile.GetObject())
        {
        case MP2::OBJ_BOAT:
            return true;

        case MP2::OBJ_HEROES:
            return toTile.GetIndex() == dst;

        default:
            break;
        }
    }

    // check corner water/coast
    if (hero.isShipMaster() &&
        direct & (Direction::TOP_LEFT | Direction::TOP_RIGHT | Direction::BOTTOM_RIGHT | Direction::BOTTOM_LEFT))
    {
        const Size wSize(world.w(), world.h());
        switch (direct)
        {
        case Direction::TOP_LEFT:
            if (Maps::isValidDirection(from, Direction::TOP, wSize) &&
                !world.GetTiles(Maps::GetDirectionIndex(from, Direction::TOP)).isWater() ||
                Maps::isValidDirection(from, Direction::LEFT, wSize) &&
                !world.GetTiles(Maps::GetDirectionIndex(from, Direction::LEFT)).isWater())
                return false;
            break;

        case Direction::TOP_RIGHT:
            if (Maps::isValidDirection(from, Direction::TOP, wSize) &&
                !world.GetTiles(Maps::GetDirectionIndex(from, Direction::TOP)).isWater() ||
                Maps::isValidDirection(from, Direction::RIGHT, wSize) &&
                !world.GetTiles(Maps::GetDirectionIndex(from, Direction::RIGHT)).isWater())
                return false;
            break;

        case Direction::BOTTOM_RIGHT:
            if (Maps::isValidDirection(from, Direction::BOTTOM, wSize) &&
                !world.GetTiles(Maps::GetDirectionIndex(from, Direction::BOTTOM)).isWater() ||
                Maps::isValidDirection(from, Direction::RIGHT, wSize) &&
                !world.GetTiles(Maps::GetDirectionIndex(from, Direction::RIGHT)).isWater())
                return false;
            break;

        case Direction::BOTTOM_LEFT:
            if (Maps::isValidDirection(from, Direction::BOTTOM, wSize) &&
                !world.GetTiles(Maps::GetDirectionIndex(from, Direction::BOTTOM)).isWater() ||
                Maps::isValidDirection(from, Direction::LEFT, wSize) &&
                !world.GetTiles(Maps::GetDirectionIndex(from, Direction::LEFT)).isWater())
                return false;
            break;

        default:
            break;
        }
    }

    return PassableToTile(hero, toTile, direct, dst);
}

uint32_t GetPenaltyFromTo(s32 from, s32 to, int direct, int pathfinding)
{
    const uint32_t cost1 = Maps::Ground::GetPenalty(from, direct, pathfinding); // penalty: for [cur] out
    const uint32_t cost2 = Maps::Ground::GetPenalty(to, Direction::Reflect(direct), pathfinding);
    // penalty: for [tmp] in
    return (cost1 + cost2) >> 1;
}

namespace
{
    struct RowInMap
    {
        int Key{};
        cell_t Value;
        int Hash{};
        int PrevIndex{};
    };

    struct PathMap
    {
        std::vector<s32> _jumpTable;
        std::vector<RowInMap> _rows;
        int _sizePow2;
        int mask{};

        void clear()
        {
            mask = (1 << _sizePow2) - 1;
            _jumpTable.clear();
            _jumpTable.reserve(mask + 1);
            for (int i = 0; i <= mask; i++)
                _jumpTable[i] = -1;
            _rows.clear();
        }

        PathMap(int sizePow2 = 10)
        {
            _sizePow2 = sizePow2;
            clear();
        }

        static int hashInt(int key)
        {
            return (key >> 4) * 31 + (key >> 3) * 7 + (key >> 5) * 7 + (key >> 6) * 13;
        }

        cell_t& get(int key)
        {
            const int hash = hashInt(key);
            const int maskedHash = hash & mask;
            RowInMap row;
            row.Key = key;
            row.Hash = hash;
            int indexToSearch = _jumpTable[maskedHash];
            if (indexToSearch == -1)
            {
                row.PrevIndex = -1;
            }
            else
            {
                do
                {
                    auto& rowRef = _rows[indexToSearch];
                    if (rowRef.Key == key)
                    {
                        return rowRef.Value;
                    }
                    indexToSearch = rowRef.PrevIndex;
                }
                while (indexToSearch != -1);
                row.PrevIndex = _jumpTable[maskedHash];
            }

            _jumpTable[maskedHash] = _rows.size();
            _rows.push_back(row);
            return _rows.back().Value;
        }
    };

    PathMap pathPointsMap2;

    int GetCurrentLength(PathMap& list, s32 from)
    {
        int res = 0;
        while (0 <= list.get(from).parent)
        {
            from = list.get(from).parent;
            ++res;
        }
        return res;
    }
}

bool Route::Path::Find(s32 to, int limit)
{
    const int pathfinding = hero->GetLevelSkill(Skill::Secondary::PATHFINDING);
    const s32 from = hero->GetIndex();

    s32 cur = from;
    s32 alt = 0;
    s32 tmp = 0;

    pathPointsMap2.clear();
    auto it1_2 = pathPointsMap2._rows.begin();
    auto it2_2 = pathPointsMap2._rows.end();

    cell_t& currCell = pathPointsMap2.get(cur);
    currCell.cost_g = 0;
    currCell.cost_t = 0;
    currCell.parent = -1;
    currCell.open = 0;

    const Directions directions = Direction::All();
    clear();

    const Size wSize(world.w(), world.h());
    LocalEvent::Get().HandleEvents(false);
    while (cur != to)
    {
        for (auto& direction : directions)
        {
            if (!Maps::isValidDirection(cur, direction, wSize))
                continue;
            tmp = Maps::GetDirectionIndex(cur, direction);
            auto& tmpItem2 = pathPointsMap2.get(tmp);
            auto& curItem2 = pathPointsMap2.get(cur);

            if (!tmpItem2.open) continue;
            const uint32_t costg = GetPenaltyFromTo(cur, tmp, direction, pathfinding);

            // new
            if (-1 == tmpItem2.parent)
            {
                if (curItem2.passbl & direction ||
                    PassableFromToTile(*hero, cur, tmp, direction, to))
                {
                    curItem2.passbl |= direction;

                    tmpItem2.direct = direction;
                    tmpItem2.cost_g = costg;
                    tmpItem2.parent = cur;
                    tmpItem2.open = 1;
                    tmpItem2.cost_d = 50 * Maps::GetApproximateDistance(tmp, to);
                    tmpItem2.cost_t = curItem2.cost_t + costg;

                    curItem2.passbl |= direction;

                    tmpItem2.direct = direction;
                    tmpItem2.cost_g = costg;
                    tmpItem2.parent = cur;
                    tmpItem2.open = 1;
                    tmpItem2.cost_d = 50 * Maps::GetApproximateDistance(tmp, to);
                    tmpItem2.cost_t = curItem2.cost_t + costg;
                }
            }
                // check alt
            else
            {
                if (tmpItem2.cost_t > curItem2.cost_t + costg &&
                    (curItem2.passbl & direction || PassableFromToTile(*hero, cur, tmp, direction, to)))
                {
                    curItem2.passbl |= direction;

                    tmpItem2.direct = direction;
                    tmpItem2.parent = cur;
                    tmpItem2.cost_g = costg;
                    tmpItem2.cost_t = curItem2.cost_t + costg;
                }
            }
        }


        pathPointsMap2.get(cur).open = 0;

        it1_2 = pathPointsMap2._rows.begin();
        alt = -1;
        tmp = MAXU16;

        // find minimal cost
        it2_2 = pathPointsMap2._rows.end();
        for (; it1_2 != it2_2; ++it1_2)
            if ((*it1_2).Value.open)
            {
                const cell_t& cell2 = (*it1_2).Value;

                if (cell2.cost_t + cell2.cost_d < tmp)
                {
                    tmp = cell2.cost_t + cell2.cost_d;
                    alt = (*it1_2).Key;
                }
            }
        // not found, and exception
        if (MAXU16 == tmp || -1 == alt) break;

        cur = alt;

        //if (0 < limit && GetCurrentLength(pathPointsMap, cur) > limit) break;
        if (0 < limit && GetCurrentLength(pathPointsMap2, cur) > limit) break;
    }

    // save path
    if (cur == to)
    {
        while (cur != from)
        {
            auto& curItem2 = pathPointsMap2.get(cur);
            push_front(Step(curItem2.parent, curItem2.direct, curItem2.cost_g));
            cur = curItem2.parent;
        }
    }
    return !empty();
}
