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

#include "interface_heroes_bar.h"
#include "gamedefs.h"
#include "interface_radar.h"
#include "interface_buttons.h"
#include "interface_icons.h"
#include "interface_status.h"
#include "interface_gamearea.h"
#include "interface_cpanel.h"
#include "text.h"
#include "interface_castle_bar.h"

enum redraw_t
{
    REDRAW_RADAR = 0x01,
    REDRAW_HEROES = 0x02,
    REDRAW_CASTLES = 0x04,
    REDRAW_BUTTONS = 0x08,
    REDRAW_STATUS = 0x10,
    REDRAW_BORDER = 0x20,
    REDRAW_GAMEAREA = 0x40,
    REDRAW_CURSOR = 0x80,

    REDRAW_ICONS = REDRAW_HEROES | REDRAW_CASTLES,
    REDRAW_ALL = 0xFF
};

class Castle;

class Heroes;

namespace Maps
{
    class Tiles;
}

namespace GameFocus
{
    enum
    {
        UNSEL = FOCUS_UNSEL, HEROES = FOCUS_HEROES, CASTLE = FOCUS_CASTLE, FIRSTHERO
    };
}

namespace Interface
{
    Castle *GetFocusCastle();

    Heroes *GetFocusHeroes();

    int GetFocusType();

    Point GetFocusCenter();

    class Basic
    {
    public:
        static Basic &Get();

        bool NeedRedraw() const;

        void SetRedraw(int);

        void Redraw(int f = 0);

        const Rect &GetScrollLeft() const;

        const Rect &GetScrollRight() const;

        const Rect &GetScrollTop() const;

        const Rect &GetScrollBottom() const;

        s32 GetDimensionDoorDestination(s32, uint32_t, bool) const;

        GameArea &GetGameArea();

        Radar &GetRadar();

        IconsPanel &GetIconsPanel();

        ButtonsArea &GetButtonsArea();

        StatusWindow &GetStatusWindow();

        ControlPanel &GetControlPanel();

        void SetFocus(Heroes *);

        void SetFocus(Castle *);

        void ResetFocus(int);

        void RedrawFocus();

        void SetHideInterface(bool);

        void EventSwitchHeroSleeping();

        void EventDefaultAction();

        static void EventOpenFocus();

        int EventSaveGame() const;

        static void EventPuzzleMaps();

        static void EventGameInfo();

        void EventSystemDialog();

        void EventNextHero();

        void EventNextTown();

        static void EventContinueMovement();

        void EventKingdomInfo() const;

        void EventCastSpell() const;

        void EventSwitchShowRadar() const;

        void EventSwitchShowStatus() const;

        void EventSwitchShowButtons() const;

        void EventSwitchShowIcons();

        void EventSwitchShowControlPanel() const;

        static void EventDebug1();

        static void EventDebug2();

        static bool EventLoadGame();

        int EventAdventureDialog();

        int EventFileDialog() const;

        static int EventEndTurn();

        static int EventExit();

        int EventDigArtifact();

        void EventKeyArrowPress(int direct);

        int StartGame();

        void MouseCursorAreaClickLeft(s32);

        void MouseCursorAreaPressRight(s32) const;

        static int GetCursorTileIndex(s32);

        static int GetCursorFocusCastle(const Castle &, const Maps::Tiles &);

        static int GetCursorFocusHeroes(const Heroes &, const Maps::Tiles &);

        static int GetCursorFocusShipmaster(const Heroes &, const Maps::Tiles &);

    private:
        Basic();

        void RedrawSystemInfo(s32, s32, uint32_t);

        void ShowPathOrStartMoveHero(Heroes *, s32);

        void MoveHeroFromArrowKeys(Heroes &hero, int direct);

        int HumanTurn(bool);

        GameArea gameArea;
        Radar radar;
        IconsPanel iconsPanel;
        ButtonsArea buttonsArea;
        StatusWindow statusWindow;
        ControlPanel controlPanel;

        HeroesBar heroesBar;

        int redraw;

        Rect scrollLeft;
        Rect scrollRight;
        Rect scrollBottom;
        Rect scrollTop;

        Text system_info;
    };
}
