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

#include <kingdom/kingdom.h>
#include "interface_border.h"

namespace Interface
{

    typedef Heroes *HEROES;
    class Basic;

    class HeroesBar : public BorderWindow
    {
    public:
        explicit HeroesBar(Basic &);

        void SetPos(s32, s32);

        void SetRedraw() const;

        void Build();

        void Redraw();

        void SetHide(bool);

        void ResetAreaSize();

        bool EventProcessing();

        void SetHeroes(HEROES* heroes, int count);

    private:
        void SavePosition();

        void Generate();

        void RedrawCursor();

        void ChangeAreaSize(const Size &);

        Basic &interface;

        Surface spriteArea;
        SpriteMove cursorArea;
        Point offset;
        bool hide;

        KingdomHeroes& kingdomHeroes;

        void SetListContent(KingdomHeroes &heroes);

        void PortraitRedraw(s16 x, s16 y, Heroes& hero, Display &display, bool isFocused);
        int getSelectedIndex() const;
        int _selectedIndex;
    };
}
