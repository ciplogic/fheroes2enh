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

#include <list>
#include "gamedefs.h"
#include "direction.h"
#include "ByteVectorReader.h"
#include "ByteVectorWriter.h"

class Heroes;

namespace Route
{
    class Step
    {
    public:
        Step() : from(-1), direction(Direction::CENTER), penalty(0)
        {}

        Step(s32 index, int dir, uint32_t cost) : from(index), direction(dir), penalty(cost)
        {}

        s32 GetIndex() const;

        uint32_t GetPenalty() const;

        s32 GetFrom() const;

        int GetDirection() const;

        bool isBad() const;

    protected:
        friend ByteVectorWriter &operator<<(ByteVectorWriter &, const Step &);
        
        friend ByteVectorReader &operator>>(ByteVectorReader &, Step &);

        s32 from;
        int direction;
        uint32_t penalty;
    };

    class Path : public list<Step>
    {
    public:
        Path(const Heroes &);

        Path(const Path &);

        Path &operator=(const Path &);

        s32 GetDestinationIndex() const;

        s32 GetLastIndex() const;

        s32 GetDestinedIndex() const;

        int GetFrontDirection() const;

        uint32_t GetFrontPenalty() const;

        uint32_t GetTotalPenalty() const;

        bool Calculate(const s32 &, int limit = -1);

        void Show()
        { hide = false; }

        void Hide()
        { hide = true; }

        void Reset();

        void PopFront();

        void PopBack();

        void RescanObstacle();

        void RescanPassable();

        bool isComplete() const;

        bool isValid() const;

        bool isShow() const
        { return !hide; }

        bool hasObstacle() const;

        string String() const;

        s32 GetAllowStep() const;

        static int GetIndexSprite(int from, int to, int mod);

    private:
        bool Find(s32, int limit = -1);

        friend ByteVectorWriter &operator<<(ByteVectorWriter &, const Path &);

        friend ByteVectorReader &operator>>(ByteVectorReader &, Path &);

        const Heroes *hero;
        s32 dst;
        bool hide;
    };
    ByteVectorWriter &operator<<(ByteVectorWriter &, const Step &);
    ByteVectorWriter &operator<<(ByteVectorWriter &, const Path &);


    ByteVectorReader &operator>>(ByteVectorReader &, Step &);
    ByteVectorReader &operator>>(ByteVectorReader &, Path &);
}

