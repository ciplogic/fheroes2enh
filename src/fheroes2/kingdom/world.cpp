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

#include <functional>
#include <algorithm>
#include "agg.h"
#include "artifact.h"
#include "resource.h"
#include "settings.h"
#include "kingdom.h"
#include "ground.h"
#include "game.h"
#include "game_over.h"
#include "maps_actions.h"
#include "world.h"
#include "rand.h"
#include <sstream>
#include <iostream>

namespace GameStatic
{
    extern uint32_t uniq;
}

MapObjects::~MapObjects()
{
    clear();
}

void MapObjects::clear()
{
    for (auto &it : *this)
        delete it.second;
    unordered_map<uint32_t, MapObjectSimple *>::clear();
}

void MapObjects::add(MapObjectSimple *obj)
{
    if (!obj) return;
    auto &map = *this;
    if (map[obj->GetUID()]) delete map[obj->GetUID()];
    map[obj->GetUID()] = obj;
}

MapObjectSimple *MapObjects::get(uint32_t uid)
{
    auto it = find(uid);
    return it != end() ? (*it).second : nullptr;
}

vector<MapObjectSimple *> MapObjects::get(const Point &pos)
{
    vector<MapObjectSimple *> res;
    for (auto &it : *this)
        if (it.second && it.second->isPosition(pos))
            res.push_back(it.second);
    return res;
}

void MapObjects::remove(const Point &pos)
{
    vector<uint32_t> uids;

    for (auto &it : *this)
        if (it.second && it.second->isPosition(pos))
            uids.push_back(it.second->GetUID());

    for (auto it : uids)
        remove(it);
}

void MapObjects::remove(uint32_t uid)
{
    const auto it = find(uid);
    if (it != end()) delete (*it).second;
    erase(it);
}

CapturedObject &CapturedObjects::Get(s32 index)
{
    auto &my = *this;
    return my[index];
}

void CapturedObjects::SetColor(s32 index, int col)
{
    Get(index).SetColor(col);
}

void CapturedObjects::Set(s32 index, int obj, int col)
{
    CapturedObject &co = Get(index);

    if (co.GetColor() != col && co.guardians.isValid())
        co.guardians.Reset();

    co.Set(obj, col);
}

uint32_t CapturedObjects::GetCount(int obj, int col) const
{
    uint32_t result = 0;

    const ObjectColor objcol(obj, col);

    for (const auto &it : *this)
        if (objcol == it.second.objcol)
            ++result;

    return result;
}

uint32_t CapturedObjects::GetCountMines(int type, int col) const
{
    uint32_t result = 0;

    const ObjectColor objcol1(MP2::OBJ_MINES, col);
    const ObjectColor objcol2(MP2::OBJ_HEROES, col);

    for (const auto &it : *this)
    {
        const ObjectColor &objcol = it.second.objcol;

        if (objcol == objcol1 || objcol == objcol2)
        {
            // scan for find mines
            const Maps::TilesAddon *addon = world.GetTiles(it.first).FindObject(MP2::OBJ_MINES);

            if (addon)
            {
                // index sprite EXTRAOVR
                if (0 == addon->index && Resource::ORE == type) ++result;
                else if (1 == addon->index && Resource::SULFUR == type) ++result;
                else if (2 == addon->index && Resource::CRYSTAL == type) ++result;
                else if (3 == addon->index && Resource::GEMS == type) ++result;
                else if (4 == addon->index && Resource::GOLD == type) ++result;
            }
        }
    }

    return result;
}

int CapturedObjects::GetColor(s32 index) const
{
    auto it = find(index);
    return it != end() ? (*it).second.GetColor() : Color::NONE;
}

void CapturedObjects::ClearFog(int colors)
{
    // clear abroad objects
    for (auto &it : *this)
    {
        const ObjectColor &objcol = it.second.objcol;

        if (objcol.isColor(colors))
        {
            int scoute = 0;

            switch (objcol.first)
            {
                case MP2::OBJ_MINES:
                case MP2::OBJ_ALCHEMYLAB:
                case MP2::OBJ_SAWMILL:
                    scoute = 2;
                    break;

                case MP2::OBJ_LIGHTHOUSE:
                    scoute = 4;
                    break; // FIXME: scoute and lighthouse

                default:
                    break;
            }

            if (scoute) Maps::ClearFog(it.first, scoute, colors);
        }
    }
}

void CapturedObjects::ResetColor(int color)
{
    for (auto &it : *this)
    {
        ObjectColor &objcol = it.second.objcol;

        if (objcol.isColor(color))
        {
            objcol.second = Color::UNUSED;
            world.GetTiles(it.first).CaptureFlags32(objcol.first, objcol.second);
        }
    }
}

Funds CapturedObjects::TributeCapturedObject(int color, int obj)
{
    Funds result;

    for (auto &it : *this)
    {
        const ObjectColor &objcol = it.second.objcol;

        if (objcol.isObject(obj) && objcol.isColor(color))
        {
            Maps::Tiles &tile = world.GetTiles(it.first);

            result += Funds(tile.QuantityResourceCount());
            tile.QuantityReset();
        }
    }

    return result;
}

World &world = World::Get();

World &World::Get()
{
    static World insideWorld;

    return insideWorld;
}

void World::Defaults()
{
    // playing kingdom
    vec_kingdoms.Init();

    // initialize all heroes
    vec_heroes.Init();

    vec_castles.Init();
}

void World::Reset()
{
    // maps tiles
    vec_tiles.clear();

    // kingdoms
    vec_kingdoms.clear();

    // event day
    vec_eventsday.clear();

    // rumors
    vec_rumors.clear();

    // castles
    vec_castles.clear();

    // heroes
    vec_heroes.clear();

    // extra
    map_captureobj.clear();
    map_actions.clear();
    map_objects.clear();

    ultimate_artifact.Reset();

    day = 0;
    week = 0;
    month = 0;

    week_current = Week::TORTOISE;
    week_next = Week::WeekRand();

    heroes_cond_wins = Heroes::UNKNOWN;
    heroes_cond_loss = Heroes::UNKNOWN;
}

/* new maps */
void World::NewMaps(uint32_t sw, uint32_t sh)
{
    Reset();
    Defaults();

    Size::w = sw;
    Size::h = sh;

    vec_tiles.resize(w() * h());

    // init all tiles
    for (auto
                 it = vec_tiles.begin(); it != vec_tiles.end(); ++it)
    {
        MP2::mp2tile_t mp2tile;

        mp2tile.tileIndex = Rand::Get(16, 19);    // index sprite ground, see ground32.til
        mp2tile.objectName1 = 0;            // object sprite level 1
        mp2tile.indexName1 = 0xff;            // index sprite level 1
        mp2tile.quantity1 = 0;
        mp2tile.quantity2 = 0;
        mp2tile.objectName2 = 0;            // object sprite level 2
        mp2tile.indexName2 = 0xff;            // index sprite level 2
        mp2tile.shape = Rand::Get(0, 3);    // shape reflect % 4, 0 none, 1 vertical, 2 horizontal, 3 any
        mp2tile.generalObject = MP2::OBJ_ZERO;
        mp2tile.indexAddon = 0;
        mp2tile.uniqNumber1 = 0;
        mp2tile.uniqNumber2 = 0;

        (*it).Init(distance(vec_tiles.begin(), it), mp2tile);
    }

    // reset current maps info
    Maps::FileInfo fi;
    fi.size_w = w();
    fi.size_h = h();

    Settings::Get().SetCurrentFileInfo(fi);
}

void World::InitKingdoms()
{
    vec_kingdoms.Init();
}

s32 World::w() const
{
    return Size::w;
}

s32 World::h() const
{
    return Size::h;
}

const Maps::Tiles &World::GetTiles(uint32_t ax, uint32_t ay) const
{
    return GetTiles(ay * w() + ax);
}

Maps::Tiles &World::GetTiles(uint32_t ax, uint32_t ay)
{
    return GetTiles(ay * w() + ax);
}

const Maps::Tiles &World::GetTiles(s32 index) const
{
    return vec_tiles[index];
}

Maps::Tiles &World::GetTiles(s32 index)
{
    return vec_tiles[index];
}

/* get kingdom */
Kingdom &World::GetKingdom(int color)
{
    return vec_kingdoms.GetKingdom(color);
}

const Kingdom &World::GetKingdom(int color) const
{
    return vec_kingdoms.GetKingdom(color);
}

/* get castle from index maps */
Castle *World::GetCastle(const Point &center)
{
    return vec_castles.Get(center);
}

const Castle *World::GetCastle(const Point &center) const
{
    return vec_castles.Get(center);
}

Heroes *World::GetHeroes(int id)
{
    return vec_heroes.Get(id);
}

const Heroes *World::GetHeroes(int id) const
{
    return vec_heroes.Get(id);
}

/* get heroes from index maps */
Heroes *World::GetHeroes(const Point &center)
{
    return vec_heroes.Get(center);
}

const Heroes *World::GetHeroes(const Point &center) const
{
    return vec_heroes.Get(center);
}

Heroes *World::GetFreemanHeroes(int race) const
{
    return vec_heroes.GetFreeman(race);
}

Heroes *World::FromJailHeroes(s32 index)
{
    return vec_heroes.FromJail(index);
}

CastleHeroes World::GetHeroes(const Castle &castle) const
{
    return CastleHeroes(vec_heroes.GetGuest(castle), vec_heroes.GetGuard(castle));
}

int World::GetDay() const
{
    return LastDay() ? DAYOFWEEK : day % DAYOFWEEK;
}

int World::GetWeek() const
{
    return LastWeek() ? WEEKOFMONTH : week % WEEKOFMONTH;
}

int World::GetMonth() const
{
    return month;
}

uint32_t World::CountDay() const
{
    return day;
}

uint32_t World::CountWeek() const
{
    return week;
}

bool World::BeginWeek() const
{
    return 1 == (day % DAYOFWEEK);
}

bool World::BeginMonth() const
{
    return 1 == (week % WEEKOFMONTH) && BeginWeek();
}

bool World::LastDay() const
{
    return (0 == (day % DAYOFWEEK));
}

bool World::LastWeek() const
{
    return (0 == (week % WEEKOFMONTH));
}

const Week &World::GetWeekType() const
{
    return week_current;
}

/* new day */
void World::NewDay()
{
    ++day;

    if (BeginWeek())
    {
        ++week;
        if (BeginMonth()) ++month;
    }

    // action new day
    vec_kingdoms.NewDay();

    // action new week
    if (BeginWeek())
    {
        NewWeek();
        vec_kingdoms.NewWeek();
    }

    // action new month
    if (BeginMonth())
    {
        NewMonth();
        vec_kingdoms.NewMonth();
    }

    // remove deprecated events
    if (day) vec_eventsday.remove_if(bind2nd(mem_fun_ref(&EventDate::isDeprecated), day - 1));
}

void World::NewWeek()
{
    // update week type
    week_current = week_next;
    const int type = LastWeek() ? Week::MonthRand() : Week::WeekRand();
    if (Week::MONSTERS == type)
        week_next = Week(type, LastWeek() ? Monster::Rand4MonthOf() : Monster::Rand4WeekOf());
    else
        week_next = Week(type);

    if (1 < week)
    {
        // update week object
        for (auto &vec_tile : vec_tiles)
            if (MP2::isWeekLife(vec_tile.GetObject(false)) ||
                MP2::OBJ_MONSTER == vec_tile.GetObject())
                vec_tile.QuantityUpdate();

        // update gray towns
        for (auto &vec_castle : vec_castles)
            if (vec_castle->GetColor() == Color::NONE) vec_castle->ActionNewWeek();
    }

    // add events
    if (Settings::Get().ExtWorldExtObjectsCaptured())
    {
        vec_kingdoms.AddTributeEvents(map_captureobj, day, MP2::OBJ_WATERWHEEL);
        vec_kingdoms.AddTributeEvents(map_captureobj, day, MP2::OBJ_WINDMILL);
        vec_kingdoms.AddTributeEvents(map_captureobj, day, MP2::OBJ_MAGICGARDEN);
    }

    // new day - reset option: "heroes: remember MP/SP for retreat/surrender result"
    for_each(vec_heroes.begin(), vec_heroes.end(),
             bind2nd(mem_fun(&Heroes::ResetModes), Heroes::SAVEPOINTS));
}

void World::NewMonth()
{
    // skip first month
    if (1 < week && week_current.GetType() == Week::MONSTERS && !Settings::Get().ExtWorldBanMonthOfMonsters())
        MonthOfMonstersAction(Monster(week_current.GetMonster()));

    // update gray towns
    for (auto &vec_castle : vec_castles)
        if (vec_castle->GetColor() == Color::NONE) vec_castle->ActionNewMonth();
}

void World::MonthOfMonstersAction(const Monster &mons)
{
    if (mons.isValid())
    {
        MapsIndexes tiles, excld;
        tiles.reserve(vec_tiles.size() / 2);
        excld.reserve(vec_tiles.size() / 2);

        const uint32_t dist = 2;
        const u8 objs[] = {MP2::OBJ_MONSTER, MP2::OBJ_HEROES, MP2::OBJ_CASTLE, MP2::OBJN_CASTLE, 0};

        MapsIndexes obja;
        // create exclude list
        {
            const MapsIndexes &objv = Maps::GetObjectsPositions(objs);

            for (int it : objv)
            {
                Maps::GetAroundIndexes(it, dist, true, obja);
                excld.insert(excld.end(), obja.begin(), obja.end());
            }
        }

        // create valid points
        for (auto it = vec_tiles.begin(); it != vec_tiles.end(); ++it)
        {
            const Maps::Tiles &tile = *it;

            if (!tile.isWater() &&
                MP2::OBJ_ZERO == tile.GetObject() &&
                tile.isPassable(nullptr, Direction::CENTER, true) &&
                excld.end() == find(excld.begin(), excld.end(), tile.GetIndex()))
            {
                tiles.push_back(tile.GetIndex());
                Maps::GetAroundIndexes(tile.GetIndex(), dist, true, obja);
                excld.insert(excld.end(), obja.begin(), obja.end());
            }
        }

        const uint32_t area = 12;
        const uint32_t maxc = (w() / area) * (h() / area);
        random_shuffle(tiles.begin(), tiles.end());
        if (tiles.size() > maxc) tiles.resize(maxc);

        for (auto &tile : tiles)
            Maps::Tiles::PlaceMonsterOnTile(vec_tiles[tile], mons, 0 /* random */);
    }
}

const string &World::GetRumors()
{
    // vec_rumors always contain values
    return *Rand::Get(vec_rumors);
}

bool TeleportCheckType(s32 index, int type)
{
    return world.GetTiles(index).QuantityTeleportType() == type;
}

bool TeleportCheckGround(s32 index, bool water)
{
    return world.GetTiles(index).isWater() == water;
}

MapsIndexes World::GetTeleportEndPoints(s32 center) const
{
    MapsIndexes result;

    if (MP2::OBJ_STONELIGHTS == GetTiles(center).GetObject(false))
    {
        result = Maps::GetObjectPositions(MP2::OBJ_STONELIGHTS, true);

        if (2 > result.size())
        {
            result.clear();
        } else
        {
            MapsIndexes::iterator itend = result.end();

            // remove if not type
            itend = remove_if(result.begin(), itend,
                              not1(bind2nd(ptr_fun(&TeleportCheckType),
                                           GetTiles(center).QuantityTeleportType())));

            // remove if index
            itend = remove(result.begin(), itend, center);

            // remove if not ground
            itend = remove_if(result.begin(), itend,
                              not1(bind2nd(ptr_fun(&TeleportCheckGround),
                                           GetTiles(center).isWater())));

            result.resize(distance(result.begin(), itend));
        }
    }

    return result;
}

/* return random teleport destination */
s32 World::NextTeleport(s32 index) const
{
    const MapsIndexes teleports = GetTeleportEndPoints(index);

    return !teleports.empty() ? *Rand::Get(teleports) : index;
}

MapsIndexes World::GetWhirlpoolEndPoints(s32 center) const
{
    if (MP2::OBJ_WHIRLPOOL == GetTiles(center).GetObject(false))
    {
        MapsIndexes whilrpools = Maps::GetObjectPositions(MP2::OBJ_WHIRLPOOL, false);
        map<s32, MapsIndexes> uniq_whirlpools;

        for (MapsIndexes::const_iterator
                     it = whilrpools.begin(); it != whilrpools.end(); ++it)
        {
            const Maps::TilesAddon *addon = GetTiles(*it).FindObjectConst(MP2::OBJ_WHIRLPOOL);
            if (addon) uniq_whirlpools[addon->uniq].push_back(*it);
        }
        whilrpools.clear();

        if (2 > uniq_whirlpools.size())
        {
            return MapsIndexes();
        }

        const Maps::TilesAddon *addon = GetTiles(center).FindObjectConst(MP2::OBJ_WHIRLPOOL);
        MapsIndexes uniqs;
        uniqs.reserve(uniq_whirlpools.size());

        if (addon)
        {
            for (auto it = uniq_whirlpools.begin(); it != uniq_whirlpools.end(); ++it)
            {
                const uint32_t &uniq = (*it).first;
                if (uniq == addon->uniq) continue;
                uniqs.push_back(uniq);
            }
        }

        return uniq_whirlpools[*Rand::Get(uniqs)];
    }

    return MapsIndexes();
}

/* return random whirlpools destination */
s32 World::NextWhirlpool(s32 index) const
{
    const MapsIndexes whilrpools = GetWhirlpoolEndPoints(index);


    return !whilrpools.empty() ? *Rand::Get(whilrpools) : index;
}

/* return message from sign */

/* return count captured object */
uint32_t World::CountCapturedObject(int obj, int col) const
{
    return map_captureobj.GetCount(obj, col);
}

/* return count captured mines */
uint32_t World::CountCapturedMines(int type, int color) const
{
    switch (type)
    {
        case Resource::WOOD:
            return CountCapturedObject(MP2::OBJ_SAWMILL, color);
        case Resource::MERCURY:
            return CountCapturedObject(MP2::OBJ_ALCHEMYLAB, color);
        default:
            break;
    }

    return map_captureobj.GetCountMines(type, color);
}

/* capture object */
void World::CaptureObject(s32 index, int color)
{
    int obj = GetTiles(index).GetObject(false);
    map_captureobj.Set(index, obj, color);

    if (MP2::OBJ_CASTLE == obj)
    {
        Castle *castle = GetCastle(Maps::GetPoint(index));
        if (castle && castle->GetColor() != color) castle->ChangeColor(color);
    }

    if (color & (Color::ALL | Color::UNUSED))
        GetTiles(index).CaptureFlags32(obj, color);
}

/* return color captured object */
int World::ColorCapturedObject(s32 index) const
{
    return map_captureobj.GetColor(index);
}

ListActions *World::GetListActions(s32 index)
{
    const auto it = map_actions.find(index);
    return it != map_actions.end() ? &(*it).second : nullptr;
}

CapturedObject &World::GetCapturedObject(s32 index)
{
    return map_captureobj.Get(index);
}

void World::ResetCapturedObjects(int color)
{
    map_captureobj.ResetColor(color);
}

void World::ClearFog(int colors)
{
    if (Settings::Get().ExtUnionsAllowViewMaps())
        colors = Players::GetPlayerFriends(colors);

    // clear abroad castles
    vec_castles.Scoute(colors);

    // clear abroad heroes
    vec_heroes.Scoute(colors);

    map_captureobj.ClearFog(colors);
}

const UltimateArtifact &World::GetUltimateArtifact() const
{
    return ultimate_artifact;
}

bool World::DiggingForUltimateArtifact(const Point &center)
{
    Maps::Tiles &tile = GetTiles(center.x, center.y);

    // puts hole sprite
    int obj = 0;
    uint32_t idx = 0;

    switch (tile.GetGround())
    {
        case Maps::Ground::WASTELAND:
            obj = 0xE4;
            idx = 70;
            break;    // ICN::OBJNCRCK
        case Maps::Ground::DIRT:
            obj = 0xE0;
            idx = 140;
            break;    // ICN::OBJNDIRT
        case Maps::Ground::DESERT:
            obj = 0xDC;
            idx = 68;
            break;    // ICN::OBJNDSRT
        case Maps::Ground::LAVA:
            obj = 0xD8;
            idx = 26;
            break;    // ICN::OBJNLAVA
        case Maps::Ground::GRASS:
        default:
            obj = 0xC0;
            idx = 9;
            break;    // ICN::OBJNGRA2
    }
    tile.AddonsPushLevel1(Maps::TilesAddon(0, GetUniq(), obj, idx));

    // reset
    if (ultimate_artifact.isPosition(tile.GetIndex()) && !ultimate_artifact.isFound())
    {
        ultimate_artifact.SetFound(true);
        return true;
    }

    return false;
}

void World::AddEventDate(const EventDate &event)
{
    vec_eventsday.push_back(event);
}

EventsDate World::GetEventsDate(int color) const
{
    EventsDate res;

    for (const auto &it : vec_eventsday)
        if (it.isAllow(color, day)) res.push_back(it);

    return res;
}

string World::DateString() const
{
    ostringstream os;
    os << "month: " << static_cast<int>(GetMonth()) <<
       ", " << "week: " << static_cast<int>(GetWeek()) << ", " << "day: " << static_cast<int>(GetDay());
    return os.str();
}

bool IsObeliskOnMaps(const Maps::Tiles &tile)
{
    return MP2::OBJ_OBELISK == tile.GetObject(false);
}

uint32_t World::CountObeliskOnMaps()
{
    uint32_t res = count_if(vec_tiles.begin(), vec_tiles.end(), IsObeliskOnMaps);
    return res ? res : 6;
}

void World::ActionForMagellanMaps(int color)
{
    for (auto &vec_tile : vec_tiles)
        if (vec_tile.isWater()) vec_tile.ClearFog(color);
}

void World::ActionToEyeMagi(int color) const
{
    MapsIndexes vec_eyes = Maps::GetObjectPositions(MP2::OBJ_EYEMAGI, true);

    if (!vec_eyes.empty())
    {
        for (MapsIndexes::const_iterator
                     it = vec_eyes.begin(); it != vec_eyes.end(); ++it)
            Maps::ClearFog(*it, GetViewDistance(Game::VIEW_MAGI_EYES), color);
    }
}

MapEvent *World::GetMapEvent(const Point &pos)
{
    vector<MapObjectSimple *> res = map_objects.get(pos);
    return !res.empty() ? static_cast<MapEvent *>(res.front()) : nullptr;
}

MapObjectSimple *World::GetMapObject(uint32_t uid)
{
    return uid ? map_objects.get(uid) : nullptr;
}

void World::RemoveMapObject(const MapObjectSimple *obj)
{
    if (obj) map_objects.remove(obj->GetUID());
}

void World::UpdateRecruits(Recruits &recruits) const
{
    if (vec_heroes.HaveTwoFreemans())
        while (recruits.GetID1() == recruits.GetID2()) recruits.SetHero2(GetFreemanHeroes());
    else
        recruits.SetHero2(nullptr);
}

const Heroes *World::GetHeroesCondWins() const
{
    return GetHeroes(heroes_cond_wins);
}

const Heroes *World::GetHeroesCondLoss() const
{
    return GetHeroes(heroes_cond_loss);
}

bool World::KingdomIsWins(const Kingdom &kingdom, int wins) const
{
    const Settings &conf = Settings::Get();

    switch (wins)
    {
        case GameOver::WINS_ALL:
            return kingdom.GetColor() == vec_kingdoms.GetNotLossColors();

        case GameOver::WINS_TOWN:
        {
            const Castle *town = GetCastle(conf.WinsMapsPositionObject());
            // check comp also wins
            return (kingdom.isControlHuman() || conf.WinsCompAlsoWins()) &&
                   (town && town->GetColor() == kingdom.GetColor());
        }

        case GameOver::WINS_HERO:
        {
            const Heroes *hero = GetHeroesCondWins();
            return (hero && Heroes::UNKNOWN != heroes_cond_wins &&
                    hero->isFreeman() &&
                    hero->GetKillerColor() == kingdom.GetColor());
        }

        case GameOver::WINS_ARTIFACT:
        {
            const KingdomHeroes &heroes = kingdom.GetHeroes();
            if (conf.WinsFindUltimateArtifact())
            {
                return (heroes.end() != find_if(heroes.begin(), heroes.end(),
                                                mem_fun(&Heroes::HasUltimateArtifact)));
            } else
            {
                const Artifact art = conf.WinsFindArtifactID();
                return (heroes.end() != find_if(heroes.begin(), heroes.end(),
                                                bind2nd(HeroHasArtifact(), art)));
            }
        }

        case GameOver::WINS_SIDE:
        {
            return !(Game::GetActualKingdomColors() & ~Players::GetPlayerFriends(kingdom.GetColor()));
        }

        case GameOver::WINS_GOLD:
            // check comp also wins
            return ((kingdom.isControlHuman() || conf.WinsCompAlsoWins()) &&
                    0 < kingdom.GetFunds().Get(Resource::GOLD) &&
                    static_cast<uint32_t>(kingdom.GetFunds().Get(Resource::GOLD)) >= conf.WinsAccumulateGold());

        default:
            break;
    }

    return false;
}

bool World::KingdomIsLoss(const Kingdom &kingdom, int loss) const
{
    const Settings &conf = Settings::Get();

    switch (loss)
    {
        case GameOver::LOSS_ALL:
            return kingdom.isLoss();

        case GameOver::LOSS_TOWN:
        {
            const Castle *town = GetCastle(conf.LossMapsPositionObject());
            return (town && town->GetColor() != kingdom.GetColor());
        }

        case GameOver::LOSS_HERO:
        {
            const Heroes *hero = GetHeroesCondLoss();
            return (hero && Heroes::UNKNOWN != heroes_cond_loss &&
                    hero->isFreeman() &&
                    hero->GetKillerColor() != kingdom.GetColor());
        }

        case GameOver::LOSS_TIME:
            return (CountDay() > conf.LossCountDays() && kingdom.isControlHuman());

        default:
            break;
    }

    return false;
}

int World::CheckKingdomWins(const Kingdom &kingdom) const
{
    const Settings &conf = Settings::Get();
    const int wins[] = {GameOver::WINS_ALL, GameOver::WINS_TOWN, GameOver::WINS_HERO, GameOver::WINS_ARTIFACT,
                        GameOver::WINS_SIDE, GameOver::WINS_GOLD, 0};

    for (uint32_t ii = 0; wins[ii]; ++ii)
        if ((conf.ConditionWins() & wins[ii]) &&
            KingdomIsWins(kingdom, wins[ii]))
            return wins[ii];

    return GameOver::COND_NONE;
}

int World::CheckKingdomLoss(const Kingdom &kingdom) const
{
    const Settings &conf = Settings::Get();

    // firs check priority: other WINS_GOLD or WINS_ARTIFACT
    if (conf.ConditionWins() & GameOver::WINS_GOLD)
    {
        int priority = vec_kingdoms.FindWins(GameOver::WINS_GOLD);
        if (priority && priority != kingdom.GetColor())
            return GameOver::LOSS_ALL;
    } else if (conf.ConditionWins() & GameOver::WINS_ARTIFACT)
    {
        int priority = vec_kingdoms.FindWins(GameOver::WINS_ARTIFACT);
        if (priority && priority != kingdom.GetColor())
            return GameOver::LOSS_ALL;
    }

    const int loss[] = {GameOver::LOSS_ALL, GameOver::LOSS_TOWN, GameOver::LOSS_HERO, GameOver::LOSS_TIME, 0};

    for (uint32_t ii = 0; loss[ii]; ++ii)
        if ((conf.ConditionLoss() & loss[ii]) &&
            KingdomIsLoss(kingdom, loss[ii]))
            return loss[ii];

    if (conf.ExtWorldStartHeroLossCond4Humans())
    {
        if (kingdom.GetFirstHeroStartCondLoss())
            return GameOver::LOSS_STARTHERO;
    }

    return GameOver::COND_NONE;
}

uint32_t World::GetUniq()
{
    return ++GameStatic::uniq;
}

StreamBase &operator<<(StreamBase &msg, const CapturedObject &obj)
{
    return msg << obj.objcol << obj.guardians << obj.split;
}

ByteVectorReader &operator>>(ByteVectorReader &msg, CapturedObject &obj)
{
    return msg >> obj.objcol >> obj.guardians >> obj.split;
}

StreamBase &operator<<(StreamBase &msg, const MapObjects &objs)
{
    msg << static_cast<uint32_t>(objs.size());
    for (const auto &it : objs)
    {
        if (!it.second)
            continue;
        const MapObjectSimple &obj = *it.second;
        msg << it.first << obj.GetType();

        switch (obj.GetType())
        {
            case MP2::OBJ_EVENT:
                msg << static_cast<const MapEvent &>(obj);
                break;

            case MP2::OBJ_SPHINX:
                msg << static_cast<const MapSphinx &>(obj);
                break;

            case MP2::OBJ_SIGN:
                msg << static_cast<const MapSign &>(obj);
                break;

            case MP2::OBJ_RESOURCE:
                msg << static_cast<const MapResource &>(obj);
                break;

            case MP2::OBJ_ARTIFACT:
                msg << static_cast<const MapArtifact &>(obj);
                break;

            case MP2::OBJ_MONSTER:
                msg << static_cast<const MapMonster &>(obj);
                break;

            default:
                msg << obj;
                break;
        }
    }
    return msg;
}

ByteVectorReader &operator>>(ByteVectorReader &msg, MapObjects &objs)
{
    uint32_t size = 0;
    msg >> size;

    objs.clear();

    for (uint32_t ii = 0; ii < size; ++ii)
    {
        s32 index;
        int type;
        msg >> index >> type;

        switch (type)
        {
            case MP2::OBJ_EVENT:
            {
                auto *ptr = new MapEvent();
                msg >> *ptr;
                objs[index] = ptr;
            }
                break;

            case MP2::OBJ_SPHINX:
            {
                auto *ptr = new MapSphinx();
                msg >> *ptr;
                objs[index] = ptr;
            }
                break;

            case MP2::OBJ_SIGN:
            {
                auto *ptr = new MapSign();
                msg >> *ptr;
                objs[index] = ptr;
            }
                break;

            case MP2::OBJ_RESOURCE:
            {
                auto *ptr = new MapResource();
                if (FORMAT_VERSION_3269 > Game::GetLoadVersion())
                    msg >> *static_cast<MapObjectSimple *>(ptr);
                else
                    msg >> *ptr;
                objs[index] = ptr;
            }
                break;

            case MP2::OBJ_ARTIFACT:
            {
                auto *ptr = new MapArtifact();
                if (FORMAT_VERSION_3269 > Game::GetLoadVersion())
                    msg >> *static_cast<MapObjectSimple *>(ptr);
                else
                    msg >> *ptr;
                objs[index] = ptr;
            }
                break;

            case MP2::OBJ_MONSTER:
            {
                auto *ptr = new MapMonster();
                if (FORMAT_VERSION_3269 > Game::GetLoadVersion())
                    msg >> *static_cast<MapObjectSimple *>(ptr);
                else
                    msg >> *ptr;
                objs[index] = ptr;
            }
                break;

            default:
            {
                auto *ptr = new MapObjectSimple();
                msg >> *ptr;
                objs[index] = ptr;
            }
                break;
        }
    }

    return msg;
}

StreamBase &operator<<(StreamBase &msg, const World &w)
{
    const Size &sz = w;

    return msg << sz <<
               w.vec_tiles <<
               w.vec_heroes <<
               w.vec_castles <<
               w.vec_kingdoms <<
               w.vec_rumors <<
               w.vec_eventsday <<
               w.map_captureobj <<
               w.ultimate_artifact <<
               w.day <<
               w.week <<
               w.month <<
               w.week_current <<
               w.week_next <<
               w.heroes_cond_wins <<
               w.heroes_cond_loss <<
               w.map_actions <<
               w.map_objects;
}


ByteVectorReader &operator>>(ByteVectorReader &msg, World &w)
{
    Size &sz = w;

    msg >> sz;
    msg >> w.vec_tiles;
    msg >> w.vec_heroes;
    msg >> w.vec_castles;
    msg >> w.vec_kingdoms;
    msg >> w.vec_rumors;
    msg >> w.vec_eventsday;
    msg >> w.map_captureobj;
    msg >> w.ultimate_artifact;
    msg >> w.day >> w.week >> w.month;
    msg >> w.week_current;
    msg >> w.week_next;
    msg >> w.heroes_cond_wins;
    msg >> w.heroes_cond_loss;
    msg >> w.map_actions;
    msg >> w.map_objects;

    // update tile passable
    for_each(w.vec_tiles.begin(), w.vec_tiles.end(),
             mem_fun_ref(&Maps::Tiles::UpdatePassable));

    // heroes postfix
    for_each(w.vec_heroes.begin(), w.vec_heroes.end(),
             mem_fun(&Heroes::RescanPathPassable));

    return msg;
}

void World::PostFixLoad()
{
}

void EventDate::LoadFromMP2(ByteVectorReader &st)
{
    // id
    if (0 != st.get())
    {
        return;
    }


    // resource
    resource.wood = st.getLE32();
    resource.mercury = st.getLE32();
    resource.ore = st.getLE32();
    resource.sulfur = st.getLE32();
    resource.crystal = st.getLE32();
    resource.gems = st.getLE32();
    resource.gold = st.getLE32();

    st.skip(2);

    // allow computer
    computer = st.getLE16();

    // day of first occurent
    first = st.getLE16();

    // subsequent occurrences
    subsequent = st.getLE16();

    st.skip(6);

    colors = 0;
    // blue
    if (st.get()) colors |= Color::BLUE;
    // green
    if (st.get()) colors |= Color::GREEN;
    // red
    if (st.get()) colors |= Color::RED;
    // yellow
    if (st.get()) colors |= Color::YELLOW;
    // orange
    if (st.get()) colors |= Color::ORANGE;
    // purple
    if (st.get()) colors |= Color::PURPLE;

    // message
    message = Game::GetEncodeString(st.toString(0));

}

bool EventDate::isDeprecated(uint32_t date) const
{
    return 0 == subsequent && first < date;
}

bool EventDate::isAllow(int col, uint32_t date) const
{
    return ((first == date ||
             (subsequent && (first < date && 0 == ((date - first) % subsequent)))) &&
            (col & colors));
}

StreamBase &operator<<(StreamBase &msg, const EventDate &obj)
{
    return msg <<
               obj.resource <<
               obj.computer <<
               obj.first <<
               obj.subsequent <<
               obj.colors <<
               obj.message;
}

ByteVectorReader &operator>>(ByteVectorReader &msg, EventDate &obj)
{
    return msg >>
               obj.resource >>
               obj.computer >>
               obj.first >>
               obj.subsequent >>
               obj.colors >>
               obj.message;
}
