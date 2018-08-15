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
#include "gamedefs.h"
#include "ByteVectorReader.h"
#include "ByteVectorWriter.h"

namespace Maps
{
    class FileInfo
    {
    public:
        FileInfo();

        FileInfo(const FileInfo &);

        FileInfo &operator=(const FileInfo &);

        static bool ReadMAP(const string &);

        bool ReadMP2(const string &);

        bool ReadSAV(const string &);

        bool operator==(const FileInfo &fi) const
        { return file == fi.file; }

        static bool NameSorting(const FileInfo &, const FileInfo &);

        static bool FileSorting(const FileInfo &, const FileInfo &);

        static bool NameCompare(const FileInfo &, const FileInfo &);

        bool isAllowCountPlayers(uint32_t) const;

        bool isMultiPlayerMap() const;

        int AllowCompHumanColors() const;

        int AllowComputerColors() const;

        int AllowHumanColors() const;

        int HumanOnlyColors() const;

        int ComputerOnlyColors() const;

        int KingdomRace(int color) const;

        int ConditionWins() const;

        int ConditionLoss() const;

        bool WinsCompAlsoWins() const;

        bool WinsAllowNormalVictory() const;

        int WinsFindArtifactID() const;

        bool WinsFindUltimateArtifact() const;

        uint32_t WinsAccumulateGold() const;

        Point WinsMapsPositionObject() const;

        Point LossMapsPositionObject() const;

        uint32_t LossCountDays() const;

        string String() const;

        void Reset();

        void FillUnions();

        string file;
        string name;
        string description;

        u16 size_w{};
        u16 size_h{};
        u8 difficulty{};
        u8 races[KINGDOMMAX]{};
        u8 unions[KINGDOMMAX]{};

        u8 kingdom_colors{};
        u8 allow_human_colors{};
        u8 allow_comp_colors{};
        u8 rnd_races{};

        u8 conditions_wins{}; // 0: wins def, 1: town, 2: hero, 3: artifact, 4: side, 5: gold
        bool comp_also_wins{};
        bool allow_normal_victory{};
        u16 wins1{};
        u16 wins2{};
        u8 conditions_loss{}; // 0: loss def, 1: town, 2: hero, 3: out time
        u16 loss1{};
        u16 loss2{};

        uint32_t localtime{};

        bool with_heroes{};
    };

    ByteVectorWriter & operator<<(ByteVectorWriter & msg, const FileInfo & fi);
    ByteVectorReader &operator>>(ByteVectorReader &, FileInfo &);
}

typedef vector<Maps::FileInfo> MapsFileInfoList;

bool PrepareMapsFileInfoList(MapsFileInfoList &, bool multi);
