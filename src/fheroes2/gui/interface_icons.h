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

#ifndef H2INTERFACE_ICONS_H
#define H2INTERFACE_ICONS_H

#include "interface_list.h"
#include "interface_border.h"

enum icons_t
{
    ICON_HEROES = 0x01, ICON_CASTLES = 0x02, ICON_ANY = ICON_HEROES | ICON_CASTLES
};

namespace Interface
{
    typedef Heroes *HEROES;
    typedef Castle *CASTLE;

    class IconsBar
    {
    public:
        IconsBar(u32 count, const Surface &sf) : iconsCount(count), marker(sf), show(true)
        {}

        virtual void SetShow(bool f)
        { show = f; };

        bool IsShow() const
        { return show; };

        void RedrawBackground(const Point &);

        u32 CountIcons() const
        { return iconsCount; }

        void SetIconsCount(u32 c)
        { iconsCount = c; }

        static u32 GetItemWidth();

        static u32 GetItemHeight();

        static bool IsVisible();

    protected:
        u32 iconsCount;
        const Surface &marker;
        bool show;
    };

    void RedrawHeroesIcon(const Heroes &, s32, s32);

    void RedrawCastleIcon(const Castle &, s32, s32);

    class HeroesIcons : public ListBox<HEROES>, public IconsBar
    {
    public:
        HeroesIcons(u32 count, const Surface &sf) : IconsBar(count, sf)
        {}

        void SetPos(s32, s32);

        void SetShow(bool) override;

    protected:
        void ActionCurrentUp() override;

        void ActionCurrentDn() override;

        void ActionListDoubleClick(HEROES &) override;

        void ActionListSingleClick(HEROES &) override;

        void ActionListPressRight(HEROES &) override;

        void RedrawItem(const HEROES &, s32 ox, s32 oy, bool current) override;

        void RedrawBackground(const Point &) override;
    };

    class CastleIcons : public ListBox<CASTLE>, public IconsBar
    {
    public:
        CastleIcons(u32 count, const Surface &sf) : IconsBar(count, sf)
        {}

        void SetPos(s32, s32);

        void SetShow(bool) override;

    protected:
        void ActionCurrentUp() override;

        void ActionCurrentDn() override;

        void ActionListDoubleClick(CASTLE &) override;

        void ActionListSingleClick(CASTLE &) override;

        void ActionListPressRight(CASTLE &) override;

        void RedrawItem(const CASTLE &, s32 ox, s32 oy, bool current) override;

        void RedrawBackground(const Point &) override;
    };

    class Basic;

    class IconsPanel : public BorderWindow
    {
    public:
        explicit IconsPanel(Basic &);

        void SetPos(s32, s32) override;

        void SavePosition() override;

        void SetRedraw() const;

        void SetRedraw(icons_t) const;

        void Redraw() override;

        bool QueueEventProcessing() override;

        u32 CountIcons() const;

        void Select(const Heroes &);

        void Select(const Castle &);

        bool IsSelected(icons_t) const;

        void ResetIcons(icons_t = ICON_ANY);

        void HideIcons(icons_t = ICON_ANY);

        void ShowIcons(icons_t = ICON_ANY);

        void RedrawIcons(icons_t = ICON_ANY);

        void SetCurrentVisible();

    private:
        Basic &interface;

        Surface sfMarker;

        CastleIcons castleIcons;
        HeroesIcons heroesIcons;
    };
}

#endif
