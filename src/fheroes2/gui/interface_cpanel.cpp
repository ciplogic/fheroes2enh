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

#include "agg.h"
#include "settings.h"
#include "game.h"
#include "game_interface.h"
#include "localevent.h"
#include "icn.h"

Interface::ControlPanel::ControlPanel(Basic &basic)
        : interface(basic)
{
    _area.w = 180;
    _area.h = 36;

    rt_radr.w = 36;
    rt_radr.h = 36;
    rt_icon.w = 36;
    rt_icon.h = 36;
    rt_bttn.w = 36;
    rt_bttn.h = 36;
    rt_stat.w = 36;
    rt_stat.h = 36;
    rt_quit.w = 36;
    rt_quit.h = 36;

    ResetTheme();
}

void Interface::ControlPanel::ResetTheme()
{
    int icn = Settings::Get().ExtGameEvilInterface() ? ICN::ADVEBTNS : ICN::ADVBTNS;

    btn_radr = AGG::GetICN(icn, 4);
    btn_icon = AGG::GetICN(icn, 0);
    btn_bttn = AGG::GetICN(icn, 12);
    btn_stat = AGG::GetICN(icn, 10);
    btn_quit = AGG::GetICN(icn, 8);

    int alpha = 130;

    btn_radr.SetAlphaMod(alpha);
    btn_icon.SetAlphaMod(alpha);
    btn_bttn.SetAlphaMod(alpha);
    btn_stat.SetAlphaMod(alpha);
    btn_quit.SetAlphaMod(alpha);
}

const Rect &Interface::ControlPanel::GetArea() const
{
    return _area;
}

void Interface::ControlPanel::SetPos(s32 ox, s32 oy)
{
    _area.x = ox;
    _area.y = oy;

    rt_radr.x = _area.x;
    rt_radr.y = _area.y;
    rt_icon.x = _area.x + 36;
    rt_icon.y = _area.y;
    rt_bttn.x = _area.x + 72;
    rt_bttn.y = _area.y;
    rt_stat.x = _area.x + 108;
    rt_stat.y = _area.y;
    rt_quit.x = _area.x + 144;
    rt_quit.y = _area.y;
}

void Interface::ControlPanel::Redraw() const
{
    Display &display = Display::Get();

    btn_radr.Blit(_area.x, _area.y, display);
    btn_icon.Blit(_area.x + 36, _area.y, display);
    btn_bttn.Blit(_area.x + 72, _area.y, display);
    btn_stat.Blit(_area.x + 108, _area.y, display);
    btn_quit.Blit(_area.x + 144, _area.y, display);
}

int Interface::ControlPanel::QueueEventProcessing() const
{
    LocalEvent &le = LocalEvent::Get();

    if (le.MouseClickLeft(rt_radr)) interface.EventSwitchShowRadar();
    else if (le.MouseClickLeft(rt_icon)) interface.EventSwitchShowIcons();
    else if (le.MouseClickLeft(rt_bttn)) interface.EventSwitchShowButtons();
    else if (le.MouseClickLeft(rt_stat)) interface.EventSwitchShowStatus();
    else if (le.MouseClickLeft(rt_quit)) return interface.EventEndTurn();

    return Game::CANCEL;
}
