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


#include <string>
#include <vector>

#include "surface.h"
#include "types.h"

class RGBA;

#include "SDL_ttf.h"

class FontTTF
{
public:
    FontTTF();

    ~FontTTF();

    TTF_Font *operator()() const
    { return ptr; }

    static void Init();

    static void Quit();

    bool Open(const std::string &, int size);

    bool isValid() const;

    void SetStyle(int) const;

    int Height() const;

    int Ascent() const;

    int Descent() const;

    int LineSkip() const;

    Surface RenderText(const std::string &, const RGBA &, bool solid /* or blended */) const;

    Surface RenderChar(char, const RGBA &, bool solid /* or blended */) const;

    Surface RenderUnicodeText(const std::vector<u16> &, const RGBA &, bool solid /* or blended */) const;

    Surface RenderUnicodeChar(u16, const RGBA &, bool solid /* or blended */) const;

protected:
    TTF_Font *ptr = nullptr;

private:
    FontTTF(const FontTTF &)
    {}

    FontTTF &operator=(const FontTTF &)
    { return *this; }
};

