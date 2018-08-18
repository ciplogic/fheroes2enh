/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#include "surface.h"

using namespace std;

class Display : public Surface
{
public:
    ~Display() override;

    static Display& Get();

    Size GetSize() const;

    string GetInfo() const;

    Size GetMaxMode(bool rotate) const;

    void SetVideoMode(int w, int h, bool);

    static void SetCaption(const char*);

    static void SetIcons(Surface&);

    void Flip();

    void Clear() const;

    void ToggleFullScreen() const;

    void Fade(int delay = 500);

    void Fade(const Surface&, const Surface&, const Point&, int level, int delay);

    void Rise(int delay = 500);

    void Rise(const Surface&, const Surface&, const Point&, int level, int delay);

    static void HideCursor();

    static void ShowCursor();

    Surface GetSurface() const override;

    Surface GetSurface(const Rect& rt) const override;

protected:

    Surface displaySurface;

    Display();
};
