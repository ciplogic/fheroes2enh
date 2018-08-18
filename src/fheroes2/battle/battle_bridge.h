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

#pragma once

namespace Battle
{
    class Unit;

    class Bridge
    {
    public:
        Bridge();

        bool NeedAction(const Unit&, s32) const;

        void Action(const Unit&, s32);

        void SetDestroy();

        void SetDown(bool);

        static void SetPassable(const Unit&);

        static bool AllowUp();

        static bool NeedDown(const Unit&, s32);

        bool isPassable(int) const;

        bool isValid() const;

        bool isDestroy() const;

        bool isDown() const;

    private:
        bool destroy;
        bool down;
    };
}
