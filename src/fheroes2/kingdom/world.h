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
#pragma once

#include <vector>
#include <map>
#include <string>
#include "ByteVectorReader.h"
#include "gamedefs.h"
#include "maps.h"
#include "maps_tiles.h"
#include "week.h"
#include "kingdom.h"
#include "castle_heroes.h"
#include "maps_objects.h"
#include "artifact_ultimate.h"
#include "maps_actions.h"
#include <unordered_map>

class Heroes;

class Castle;

class Kingdom;

class Recruits;

class Radar;

class MapObjectSimple;


struct ListActions : public vector<up<ActionSimple>>
{
};

struct MapObjects : public unordered_map<uint32_t, MapObjectSimple *>
{
    ~MapObjects();

    void clear();

    void add(MapObjectSimple *);

    vector<MapObjectSimple *> get(const Point &);

    MapObjectSimple *get(uint32_t uid);

    void remove(const Point &);

    void remove(uint32_t uid);
};

typedef map<s32, ListActions> MapActions;

struct CapturedObject
{
    ObjectColor objcol;
    Troop guardians;
    int split;

    CapturedObject() : split(1)
    {}

    int GetSplit() const
    { return split; }

    int GetObject() const
    { return objcol.first; }

    int GetColor() const
    { return objcol.second; }

    Troop &GetTroop()
    { return guardians; }

    void Set(int obj, int col)
    { objcol = ObjectColor(obj, col); }

    void SetColor(int col)
    { objcol.second = col; }

    void SetSplit(int spl)
    { split = spl; }

    bool GuardiansProtected() const
    { return guardians.isValid(); }
};

struct CapturedObjects : map<s32, CapturedObject>
{
    void Set(s32, int, int);

    void SetColor(s32, int);

    void ClearFog(int);

    void ResetColor(int);

    CapturedObject &Get(s32);

    Funds TributeCapturedObject(int col, int obj);

    uint32_t GetCount(int, int) const;

    uint32_t GetCountMines(int, int) const;

    int GetColor(s32) const;
};

struct EventDate
{
    EventDate() : computer(false), first(0), subsequent(0), colors(0)
    {}

    void LoadFromMP2(ByteVectorReader &);

    bool isAllow(int color, uint32_t date) const;

    bool isDeprecated(uint32_t date) const;

    Funds resource;
    bool computer;
    uint32_t first;
    uint32_t subsequent;
    int colors;
    string message;
};

ByteVectorWriter &operator<<(ByteVectorWriter &, const EventDate &);
ByteVectorReader &operator>>(ByteVectorReader &, EventDate &);

typedef vector<string> Rumors;
typedef list<EventDate> EventsDate;
typedef vector<Maps::Tiles> MapsTiles;

class World : protected Size
{
public:
    ~World()
    { Reset(); }

    bool LoadMapMP2(const string &);

    bool LoadMapMAP(const string &) const;

    void NewMaps(uint32_t, uint32_t);

    static World &Get();

    s32 w() const;

    s32 h() const;

    const Maps::Tiles &GetTiles(uint32_t, uint32_t) const;

    Maps::Tiles &GetTiles(uint32_t, uint32_t);

    const Maps::Tiles &GetTiles(s32) const;

    Maps::Tiles &GetTiles(s32);

    void InitKingdoms();

    Kingdom &GetKingdom(int color);

    const Kingdom &GetKingdom(int color) const;

    const Castle *GetCastle(const Point &) const;

    Castle *GetCastle(const Point &);

    const Heroes *GetHeroes(int /* hero id */) const;

    Heroes *GetHeroes(int /* hero id */);

    const Heroes *GetHeroes(const Point &) const;

    Heroes *GetHeroes(const Point &);

    Heroes *FromJailHeroes(s32) const;

    Heroes *GetFreemanHeroes(int race = 0) const;

    const Heroes *GetHeroesCondWins() const;

    const Heroes *GetHeroesCondLoss() const;

    CastleHeroes GetHeroes(const Castle &) const;

    const UltimateArtifact &GetUltimateArtifact() const;

    bool DiggingForUltimateArtifact(const Point &);

    int GetDay() const;

    int GetWeek() const;

    int GetMonth() const;

    uint32_t CountDay() const;

    uint32_t CountWeek() const;

    bool BeginWeek() const;

    bool BeginMonth() const;

    bool LastDay() const;

    bool LastWeek() const;

    const Week &GetWeekType() const;

    string DateString() const;

    void NewDay();

    void NewWeek();

    void NewMonth();

    const string &GetRumors() const;

    s32 NextTeleport(s32) const;

    MapsIndexes GetTeleportEndPoints(s32) const;

    s32 NextWhirlpool(s32) const;

    MapsIndexes GetWhirlpoolEndPoints(s32) const;


    void CaptureObject(s32, int col);

    uint32_t CountCapturedObject(int obj, int col) const;

    uint32_t CountCapturedMines(int type, int col) const;

    uint32_t CountObeliskOnMaps();

    int ColorCapturedObject(s32) const;

    void ResetCapturedObjects(int);

    CapturedObject &GetCapturedObject(s32);

    ListActions *GetListActions(s32);

    void ActionForMagellanMaps(int color);

    void ActionToEyeMagi(int color) const;

    void ClearFog(int color);

    void UpdateRecruits(Recruits &) const;


    int CheckKingdomWins(const Kingdom &) const;

    bool KingdomIsWins(const Kingdom &, int wins) const;

    int CheckKingdomLoss(const Kingdom &) const;

    bool KingdomIsLoss(const Kingdom &, int loss) const;

    void AddEventDate(const EventDate &);

    EventsDate GetEventsDate(int color) const;

    MapEvent *GetMapEvent(const Point &);

    MapObjectSimple *GetMapObject(uint32_t uid);

    void RemoveMapObject(const MapObjectSimple *);

    static uint32_t GetUniq();

    void PostFixLoad() const;

private:
    World() : Size(0, 0), day(0), week(0), month(0), heroes_cond_wins(0), heroes_cond_loss(0)
    {};

    void Defaults();

    void Reset();

    void MonthOfMonstersAction(const Monster &);

    void PostLoad();

private:
    friend class Radar;

    friend ByteVectorWriter &operator<<(ByteVectorWriter &, const World &);
    
    friend ByteVectorReader &operator>>(ByteVectorReader &, World &);
public:
    MapsTiles vec_tiles;
    AllHeroes vec_heroes;
    AllCastles vec_castles;
    Kingdoms vec_kingdoms;
    Rumors vec_rumors;
    EventsDate vec_eventsday;

    // index, object, color
    CapturedObjects map_captureobj;

    UltimateArtifact ultimate_artifact;

    uint32_t day;
    uint32_t week;
    uint32_t month;

    Week week_current;
    Week week_next;

    int heroes_cond_wins;
    int heroes_cond_loss;

    MapActions map_actions;
    MapObjects map_objects;
};

ByteVectorWriter &operator<<(ByteVectorWriter &, const CapturedObject &);
ByteVectorReader &operator>>(ByteVectorReader &, CapturedObject &);

ByteVectorWriter &operator<<(ByteVectorWriter &, const World &);
ByteVectorReader &operator>>(ByteVectorReader &, World &);

ByteVectorReader &operator>>(ByteVectorReader &, MapActions &);

ByteVectorWriter &operator<<(ByteVectorWriter &, const ListActions &);
ByteVectorReader &operator>>(ByteVectorReader &, ListActions &);

ByteVectorWriter &operator<<(ByteVectorWriter &msg, const MapObjects &objs);
ByteVectorReader &operator>>(ByteVectorReader &, MapObjects &);

extern World &world;

