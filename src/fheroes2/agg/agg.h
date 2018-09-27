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
#include <utility>

#include "gamedefs.h"
#include "sprite.h"

namespace AGG
{
    class ICNSprite : public std::pair<Surface, Surface> /* first: image with out alpha, second: shadow with alpha */
    {
    public:
        ICNSprite() = default;

        ICNSprite(const Surface& sf1, const Surface& sf2) : std::pair<Surface, Surface>(sf1, sf2)
        {
        }

        bool isValid() const;

        sp<Sprite> CreateSprite(bool reflect, bool shadow) const;

        Surface First() const
        {
            return first;
        }

        Surface Second() const
        {
            return second;
        }

        Point offset{};
    };

    bool Init();

    void Quit();

    int PutICN(const Sprite&, bool init_reflect = false);

    Sprite GetICN(int icn, uint32_t index, bool reflect = false);

    uint32_t GetICNCount(int icn);

    Surface GetTIL(int til, uint32_t index, uint32_t shape);

    Surface GetLetter(uint32_t ch, uint32_t ft);

#ifdef WITH_TTF

    Surface GetUnicodeLetter(uint32_t ch, uint32_t ft);

    uint32_t GetFontHeight(bool small);

#endif

    void LoadLOOPXXSounds(const std::vector<int>&);

    void PlaySound(int m82);

    void PlayMusic(int mus, bool loop = true);

    void ResetMixer();

    RGBA GetPaletteColor(uint32_t index);

    void DrawPointFast(Surface& srf, int x, int y, u8 color);

    ICNSprite RenderICNSprite(int, uint32_t);

    void RenderICNSprite(int icn, uint32_t index, const Rect& srt, const Point& dpt, Surface& dst);
}
