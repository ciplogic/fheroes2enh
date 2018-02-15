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

#include <cmath>

#include "interface_castle_bar.h"
#include "interface_icons.h"
#include "agg.h"
#include "settings.h"
#include "game.h"
#include "game_interface.h"
#include "world.h"

namespace
{

    RGBA colBright(192, 192, 20);
    RGBA colDark(128, 128, 10);

    int posLeftSpacing = 80;
    int shiftBottom = 160;
    int spaceTiling = 36;
}

/* constructor */
Interface::CastleBar::CastleBar(Interface::Basic &basic)
        : BorderWindow(Rect(0, 0, RADARWIDTH, RADARWIDTH)),
          interface(basic), hide(true),
          kingdomCastles(KingdomCastles::GetNull())
{
}

void Interface::CastleBar::SavePosition()
{
    Settings::Get().SetPosRadar(GetRect());
}

void Interface::CastleBar::SetPos(s32 ox, s32 oy)
{
    SetPosition(ox, oy);
}

/* construct gui */
void Interface::CastleBar::Build()
{
    Generate();
    RedrawCursor();
}

/* generate mini maps */
void Interface::CastleBar::Generate()
{
}

void Interface::CastleBar::SetHide(bool f)
{
    hide = f;
}

void Interface::CastleBar::SetRedraw() const
{
}

void Interface::CastleBar::Redraw()
{
    auto &kingdom = world.GetKingdom(Settings::Get().CurrentColor());
    if (!kingdom.isControlHuman())
        return;
    Display &display = Display::Get();


    const Settings &conf = Settings::Get();
    const Rect &area = GetArea();


    // portrait
    Point dst_pt;
    dst_pt.x = posLeftSpacing;
    dst_pt.y = display.h()-shiftBottom;
    SetListContent(kingdom.GetCastles());

    auto _selectedIndex = getSelectedIndex();
    SetPos(dst_pt.x, dst_pt.y - kingdomCastles.size()*::spaceTiling);
    int pos = 0;
    for (Castle* &castle : this->kingdomCastles)
    {
        PortraitRedraw(dst_pt.x, dst_pt.y, *castle, display, pos == _selectedIndex);
        dst_pt.y -= spaceTiling;
        pos++;
    }
}

int Interface::CastleBar::getSelectedIndex() const
{
    Player &player = *Settings::Get().GetPlayers().GetCurrent();
    Focus &focus = player.GetFocus();
    auto *selectedHero = focus.GetCastle();

    if (!selectedHero)
        return -1;
    int selected = 0;
    for (Castle* &castle:this->kingdomCastles)
    {
        if (castle->GetName() == selectedHero->GetName())
        {
            break;
        }
        selected++;
    }
    return selected;
}

/* redraw CastleBar cursor */
void Interface::CastleBar::RedrawCursor()
{
    const Settings &conf = Settings::Get();

}

bool Interface::CastleBar::EventProcessing()
{
    LocalEvent &le = LocalEvent::Get();

    Display &display = Display::Get();
   
    auto& kingdom =world.GetKingdom(Settings::Get().CurrentColor());
    if(!kingdom.isControlHuman())
        return false;

    int posBottom = display.h() - shiftBottom + ::spaceTiling;

    SetListContent(kingdom.GetCastles());
    if (le.MouseClickLeft())
    {
        const Point &pt = le.GetMousePressLeft();
        if(pt.x<::posLeftSpacing)
            return false;
        if (pt.x>::posLeftSpacing+32)
            return false;
        if (pt.y>posBottom+spaceTiling)
            return false;
        uint32_t index = (posBottom - pt.y) / spaceTiling;

        if(index<kingdomCastles.size())
        {
            Castle *heroClick = kingdomCastles[index];
            interface.SetFocus(heroClick);
            return true;
        }

    }
    return false;
}

void Interface::CastleBar::ResetAreaSize()
{
    ChangeAreaSize(Size(RADARWIDTH, RADARWIDTH));
}

void Interface::CastleBar::ChangeAreaSize(const Size &newSize)
{
    if (newSize == area) return;
    const Rect &rect = GetRect();
    Cursor::Get().Hide();
    SetPosition(rect.x < 0 ? 0 : rect.x, rect.y < 0 ? 0 : rect.y, newSize.w, newSize.h);
    Generate();
    RedrawCursor();
    interface.GetGameArea().SetRedraw();
}

void Interface::CastleBar::SetHeroes(Interface::HEROES *heroes, int count)
{
}

void Interface::CastleBar::SetListContent(KingdomCastles &castles)
{
    kingdomCastles = castles;
}

namespace
{
    void rectangleFill(Surface& srf, int heightBevel, int heroLevel, bool isFocused, Point topLeft)
    {
        Surface srfTop(Size(srf.w(), srf.h()), true);
        int wid = srfTop.w() - 1;
        int hgt = srfTop.h() - 1;
        if (isFocused) 
        {
            for (int i = 0; i < heightBevel; i++) 
            {
                srfTop.DrawLine(Point(i, i), Point(wid - i, i), colBright);
                srfTop.DrawLine(Point(i, i), Point(i, hgt-i), colBright);

                srfTop.DrawLine(Point(i, hgt - i), Point(wid, hgt - i), colDark);
                srfTop.DrawLine(Point(wid - i, i), Point(wid - i, hgt), colDark);

            }
        }
        Text textPlus = { Int2Str(heroLevel) };
        int textW = textPlus.w();
        int xText = srfTop.w() - textW-8;
        int yText = srfTop.h() - textPlus.h() - 8;
        Size textSize(textPlus.w(), textPlus.h());
        Point textPoint(xText, yText);
        Rect rectArea(textPoint, textSize);
        RGBA white(40, 40, 185);
        srfTop.FillRect(rectArea, white);

        srfTop.SetAlphaMod(180);        
        if (isFocused)
        {
            srfTop.Blit(srf);
        }
        if (!isFocused)
            srf.SetAlphaMod(120);
        srf.Blit(topLeft.x, topLeft.y, Display::Get());
        srfTop.Blit(topLeft.x, topLeft.y, Display::Get());
        textPoint += topLeft;
        textPlus.Blit(textPoint);

    }
}

void Interface::CastleBar::PortraitRedraw(s16 x, s16 y, Castle& hero, Display &display, bool isFocused)
{
    Surface srfBack(Size(64,64),true);
    Interface::RedrawCastleIcon(hero, 0, 0, srfBack);
    if (!isFocused)
        srfBack.SetAlphaMod(120);
    srfBack.Blit(x, y, display);
}
