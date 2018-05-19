/***************************************************************************
 *   Copyright (C) 2013 by Andrey Afletdinov <fheroes2@gmail.com>          *
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
#include "color.h"
#include "settings.h"
#include "mp2.h"
#include "game.h"
#include "army_troop.h"
#include "maps_objects.h"

#define SIZEMESSAGE 400

StreamBase &operator<<(StreamBase &msg, const MapObjectSimple &obj)
{
    return msg << obj.type << obj.uid << static_cast<const MapPosition &>(obj);
}

ByteVectorWriter &operator<<(ByteVectorWriter &msg, const MapObjectSimple &obj)
{
    return msg << obj.type << obj.uid << static_cast<const MapPosition &>(obj);
}

ByteVectorReader &operator>>(ByteVectorReader &msg, MapObjectSimple &obj)
{
    return msg >> obj.type >> obj.uid >> static_cast<MapPosition &>(obj);
}

MapEvent::MapEvent() : MapObjectSimple(MP2::OBJ_EVENT), computer(false), cancel(true), colors(0)
{
}

void MapEvent::LoadFromMP2(s32 index, ByteVectorReader &st)
{
    // id
    if (1 == st.get())
    {
        SetIndex(index);

        // resource
        resources.wood = st.getLE32();
        resources.mercury = st.getLE32();
        resources.ore = st.getLE32();
        resources.sulfur = st.getLE32();
        resources.crystal = st.getLE32();
        resources.gems = st.getLE32();
        resources.gold = st.getLE32();

        // artifact
        artifact = st.getLE16();

        // allow computer
        computer = st.get();

        // cancel event after first visit
        cancel = st.get();

        st.skip(10);

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
}

void MapEvent::SetVisited(int color)
{
    if (cancel)
        colors = 0;
    else
        colors &= ~color;
}

bool MapEvent::isAllow(int col) const
{
    return col & colors;
}

MapSphinx::MapSphinx() : MapObjectSimple(MP2::OBJ_SPHINX), valid(false)
{
}

void MapSphinx::LoadFromMP2(s32 index, ByteVectorReader &st)
{
    // id
    if (0 != st.get())
        return;
    SetIndex(index);

    // resource
    resources.wood = st.getLE32();
    resources.mercury = st.getLE32();
    resources.ore = st.getLE32();
    resources.sulfur = st.getLE32();
    resources.crystal = st.getLE32();
    resources.gems = st.getLE32();
    resources.gold = st.getLE32();

    // artifact
    artifact = st.getLE16();

    // count answers
    uint32_t count = st.get();

    // answers
    for (uint32_t i = 0; i < 8; ++i)
    {
        string answer = Game::GetEncodeString(st.toString(13));

        if (count-- && !answer.empty())
            answers.push_back(StringLower(answer));
    }

    // message
    message = Game::GetEncodeString(st.toString(0));

    valid = true;
}

bool MapSphinx::AnswerCorrect(const string &answer)
{
    return answers.end() != find(answers.begin(), answers.end(), StringLower(answer));
}

void MapSphinx::SetQuiet()
{
    valid = false;
    artifact = Artifact::UNKNOWN;
    resources.Reset();
}


ByteVectorWriter &operator<<(ByteVectorWriter &msg, const MapEvent &obj)
{
    return msg <<
        static_cast<const MapObjectSimple &>(obj) <<
        obj.resources <<
        obj.artifact <<
        obj.computer <<
        obj.cancel <<
        obj.colors <<
        obj.message;
}

ByteVectorReader &operator>>(ByteVectorReader &msg, MapEvent &obj)
{
    return msg >>
               static_cast<MapObjectSimple &>(obj) >>
               obj.resources >>
               obj.artifact >>
               obj.computer >>
               obj.cancel >>
               obj.colors >>
               obj.message;
}

ByteVectorWriter &operator<<(ByteVectorWriter &msg, const MapSphinx &obj)
{
    return msg <<
               static_cast<const MapObjectSimple &>(obj) <<
               obj.resources <<
               obj.artifact <<
               obj.answers <<
               obj.message <<
               obj.valid;
}

ByteVectorReader &operator>>(ByteVectorReader &msg, MapSphinx &obj)
{
    return msg >>
               static_cast<MapObjectSimple &>(obj) >>
               obj.resources >>
               obj.artifact >>
               obj.answers >>
               obj.message >>
               obj.valid;
}

MapSign::MapSign() : MapObjectSimple(MP2::OBJ_SIGN)
{
}

MapSign::MapSign(s32 index, const string &msg) : MapObjectSimple(MP2::OBJ_SIGN)
{
    SetIndex(index);
    message = msg;
}

void MapSign::LoadFromMP2(s32 index, ByteVectorReader &st)
{
    st.skip(9);
    message = st.toString(0);

    SetIndex(index);
    message = Game::GetEncodeString(message);
}

ByteVectorWriter &operator<<(ByteVectorWriter &msg, const MapSign &obj)
{
    return msg <<
               static_cast<const MapObjectSimple &>(obj) <<
               obj.message;
}

ByteVectorReader &operator>>(ByteVectorReader &msg, MapSign &obj)
{
    return msg >>
               static_cast<MapObjectSimple &>(obj) >>
               obj.message;
}

MapResource::MapResource() : MapObjectSimple(MP2::OBJ_RESOURCE)
{
}

ByteVectorWriter &operator<<(ByteVectorWriter &msg, const MapResource &obj)
{
    return msg <<
               static_cast<const MapObjectSimple &>(obj) <<
               obj.resource;
}

ByteVectorReader &operator>>(ByteVectorReader &msg, MapResource &obj)
{
    return msg >>
               static_cast<MapObjectSimple &>(obj) >>
               obj.resource;
}

MapArtifact::MapArtifact() : MapObjectSimple(MP2::OBJ_ARTIFACT), condition(0), extended(0)
{
}

Funds MapArtifact::QuantityFunds() const
{
    switch (condition)
    {
        case 1:
            return {QuantityResourceCount()};
        case 2:
            return Funds(Resource::GOLD, 2500) + Funds(QuantityResourceCount());
        case 3:
            return Funds(Resource::GOLD, 3000) + Funds(QuantityResourceCount());
        default:
            break;
    }

    return Funds();
}

ResourceCount MapArtifact::QuantityResourceCount() const
{
    switch (condition)
    {
        case 1:
            return {Resource::GOLD, 2000};
        case 2:
            return ResourceCount(extended, 3);
        case 3:
            return ResourceCount(extended, 5);
        default:
            break;
    }

    return ResourceCount();
}

ByteVectorWriter &operator<<(ByteVectorWriter &msg, const MapArtifact &obj)
{
    return msg <<
               static_cast<const MapObjectSimple &>(obj) <<
               obj.artifact << obj.condition << obj.extended;
}

ByteVectorReader &operator>>(ByteVectorReader &msg, MapArtifact &obj)
{
    return msg >>
               static_cast<MapObjectSimple &>(obj) >>
               obj.artifact >> obj.condition >> obj.extended;
}

MapMonster::MapMonster() : MapObjectSimple(MP2::OBJ_MONSTER), condition(0), count(0)
{
}

Troop MapMonster::QuantityTroop() const
{
    return Troop(monster, count);
}

bool MapMonster::JoinConditionSkip() const
{
    return Monster::JOIN_CONDITION_SKIP == condition;
}

bool MapMonster::JoinConditionMoney() const
{
    return Monster::JOIN_CONDITION_MONEY == condition;
}

bool MapMonster::JoinConditionFree() const
{
    return Monster::JOIN_CONDITION_FREE == condition;
}

bool MapMonster::JoinConditionForce() const
{
    return Monster::JOIN_CONDITION_FORCE == condition;
}

ByteVectorWriter &operator<<(ByteVectorWriter &msg, const MapMonster &obj)
{
    return msg <<
               static_cast<const MapObjectSimple &>(obj) <<
               obj.monster << obj.condition << obj.count;
}

ByteVectorReader &operator>>(ByteVectorReader &msg, MapMonster &obj)
{
    return msg >>
               static_cast<MapObjectSimple &>(obj) >>
               obj.monster >> obj.condition >> obj.count;
}
