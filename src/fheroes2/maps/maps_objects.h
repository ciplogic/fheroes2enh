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

#pragma once


#include "ByteVectorReader.h"
#include <vector>
#include <string>
#include "resource.h"
#include "monster.h"
#include "artifact.h"
#include "position.h"
#include "pairs.h"

class MapObjectSimple : public MapPosition
{
public:
    explicit MapObjectSimple(int v = 0) : uid(0), type(v)
    {
    }

    virtual ~MapObjectSimple() = default;

    int GetType() const
    {
        return type;
    }

    uint32_t GetUID() const
    {
        return uid;
    }

    void SetUID(uint32_t v)
    {
        uid = v;
    }

    uint32_t uid;
    int type;
};

ByteVectorWriter& operator<<(ByteVectorWriter&, const MapObjectSimple&);
ByteVectorReader& operator>>(ByteVectorReader&, MapObjectSimple&);

struct MapEvent : MapObjectSimple
{
    MapEvent();

    void LoadFromMP2(s32 index, ByteVectorReader&);

    bool isAllow(int color) const;

    void SetVisited(int color);

    Funds resources;
    Artifact artifact;
    bool computer;
    bool cancel;
    int colors;
    string message;
};

ByteVectorWriter& operator<<(ByteVectorWriter&, const MapEvent&);

ByteVectorReader& operator>>(ByteVectorReader&, MapEvent&);

typedef vector<string> RiddleAnswers;

struct MapSphinx : MapObjectSimple
{
    MapSphinx();

    void LoadFromMP2(s32 index, ByteVectorReader&);

    bool AnswerCorrect(const string& answer);

    void SetQuiet();

    Funds resources;
    Artifact artifact;
    RiddleAnswers answers;
    string message;
    bool valid;
};

ByteVectorWriter& operator<<(ByteVectorWriter&, const MapSphinx&);

ByteVectorReader& operator>>(ByteVectorReader&, MapSphinx&);

struct MapSign : MapObjectSimple
{
    MapSign();

    MapSign(s32 index, const string&);

    void LoadFromMP2(s32 index, ByteVectorReader&);

    string message;
};

ByteVectorWriter& operator<<(ByteVectorWriter&, const MapSign&);

ByteVectorReader& operator>>(ByteVectorReader&, MapSign&);

struct MapResource : MapObjectSimple
{
    MapResource();

    ResourceCount resource;
};

ByteVectorWriter& operator<<(ByteVectorWriter&, const MapResource&);

ByteVectorReader& operator>>(ByteVectorReader&, MapResource&);

struct MapArtifact : MapObjectSimple
{
    MapArtifact();

    Artifact artifact;
    int condition;
    int extended;

    Funds QuantityFunds() const;

    ResourceCount QuantityResourceCount() const;
};

ByteVectorWriter& operator<<(ByteVectorWriter&, const MapArtifact&);

ByteVectorReader& operator>>(ByteVectorReader&, MapArtifact&);

struct MapMonster : MapObjectSimple
{
    MapMonster();

    Monster monster;

    int condition;
    int count;

    Troop QuantityTroop() const;

    bool JoinConditionSkip() const;

    bool JoinConditionMoney() const;

    bool JoinConditionFree() const;

    bool JoinConditionForce() const;
};

ByteVectorWriter& operator<<(ByteVectorWriter&, const MapMonster&);

ByteVectorReader& operator>>(ByteVectorReader&, MapMonster&);
