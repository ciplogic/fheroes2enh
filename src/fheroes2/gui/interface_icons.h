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

#include "interface_list.h"
#include "interface_border.h"

enum icons_t
{
    ICON_HEROES = 0x01,
    ICON_CASTLES = 0x02,
    ICON_ANY = ICON_HEROES | ICON_CASTLES
};

namespace Interface
{
    typedef Heroes* HEROES;
    typedef Castle* CASTLE;

    class IconsBar
    {
    public:
        IconsBar(uint32_t count, const Surface& sf) : iconsCount(count), marker(sf), show(true)
        {
        }

        void SetShow(bool f)
        {
            show = f;
        }

        bool IsShow() const
        {
            return show;
        };

        void RedrawBackground(const Point&) const;

        uint32_t CountIcons() const
        {
            return iconsCount;
        }

        void SetIconsCount(uint32_t c)
        {
            iconsCount = c;
        }

        static uint32_t GetItemWidth();

        static uint32_t GetItemHeight();

        static bool IsVisible();

    protected:
        uint32_t iconsCount;
        const Surface& marker;
        bool show;
    };

    void RedrawCastleIcon(const Castle&, s32, s32, Surface& destSurface);
    void RedrawHeroesIcon(const Heroes&, s32, s32);


    class HeroesIcons : public ListBox<HEROES>, public IconsBar
    {
    public:
        HeroesIcons(uint32_t count, const Surface& sf) : IconsBar(count, sf)
        {
        }

        void SetPos(s32, s32);

        void SetShow(bool);

    protected:
        void ActionCurrentUp();

        void ActionCurrentDn();

        void ActionListDoubleClick(HEROES&);

        void ActionListSingleClick(HEROES&);

        void ActionListPressRight(HEROES&);

        void RedrawItem(const HEROES&, s32 ox, s32 oy, bool current);

        void RedrawBackground(const Point&);
    };

    class HeroesBottomIcons : public ListBox<HEROES>, public IconsBar
    {
    public:
        HeroesBottomIcons(uint32_t count, const Surface& sf) : IconsBar(count, sf)
        {
        }


        static void SetShow(bool);

        static void SetHero(HEROES const pHeroes);

    protected:
        void ActionCurrentUp();

        void ActionCurrentDn();

        void ActionListDoubleClick(HEROES&);

        void ActionListSingleClick(HEROES&);

        void ActionListPressRight(HEROES&);

        void RedrawItem(const HEROES&, s32 ox, s32 oy, bool current);

        void RedrawBackground(const Point&);
    };

    class CastleIcons : public ListBox<CASTLE>, public IconsBar
    {
    public:
        CastleIcons(uint32_t count, const Surface& sf) : IconsBar(count, sf)
        {
        }

        void SetPos(s32, s32);

        void SetShow(bool);

    protected:
        void ActionCurrentUp();

        void ActionCurrentDn();

        void ActionListDoubleClick(CASTLE&);

        void ActionListSingleClick(CASTLE&);

        void ActionListPressRight(CASTLE&);

        void RedrawItem(const CASTLE&, s32 ox, s32 oy, bool current);

        void RedrawBackground(const Point&);
    };

    class Basic;

    class IconsPanel : public BorderWindow
    {
    public:
        explicit IconsPanel(Basic&);

        void SetPos(s32, s32);

        void SavePosition();

        void SetRedraw() const;

        void SetRedraw(icons_t) const;

        void Redraw();

        void QueueEventProcessing();

        uint32_t CountIcons() const;

        void Select(const Heroes&);

        void Select(const Castle&);

        bool IsSelected(icons_t) const;

        void ResetIcons(icons_t = ICON_ANY);

        void HideIcons(icons_t = ICON_ANY);

        void ShowIcons(icons_t = ICON_ANY);

        void RedrawIcons(icons_t = ICON_ANY);

        void SetCurrentVisible();

    private:
        Basic& interface;

        Surface sfMarker;

        Surface sfHeroes;

        CastleIcons castleIcons;
        HeroesIcons heroesIcons;
        HeroesBottomIcons heroesBottom;
    };
}
