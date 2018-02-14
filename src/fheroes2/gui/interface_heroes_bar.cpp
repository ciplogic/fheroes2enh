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

#include "interface_heroes_bar.h"
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
    int spaceTiling = 120;
}

/* constructor */
Interface::HeroesBar::HeroesBar(Basic &basic)
        : BorderWindow(Rect(0, 0, RADARWIDTH, RADARWIDTH)),
          interface(basic), hide(true),
          kingdomHeroes(KingdomHeroes::GetNull())
{
}

void Interface::HeroesBar::SavePosition()
{
    Settings::Get().SetPosRadar(GetRect());
}

void Interface::HeroesBar::SetPos(s32 ox, s32 oy)
{
    SetPosition(ox, oy);
}

/* construct gui */
void Interface::HeroesBar::Build()
{
    Generate();
    RedrawCursor();
}

/* generate mini maps */
void Interface::HeroesBar::Generate()
{
}

void Interface::HeroesBar::SetHide(bool f)
{
    hide = f;
}

void Interface::HeroesBar::SetRedraw() const
{
}

void Interface::HeroesBar::Redraw()
{
    auto &kingdom = world.GetKingdom(Settings::Get().CurrentColor());
    if (!kingdom.isControlHuman())
        return;
    Display &display = Display::Get();

    SetPos(posLeftSpacing, display.h() - 110);

    const Settings &conf = Settings::Get();
    const Rect &area = GetArea();


    // portrait
    Point dst_pt;
    dst_pt.x = area.x;
    dst_pt.y = area.y;
    SetListContent(kingdom.GetHeroes());

    this->_selectedIndex = getSelectedIndex();
    int pos = 0;
    for (Heroes *&hero:kingdomHeroes)
    {
        PortraitRedraw(dst_pt.x, dst_pt.y, *hero, display, pos == _selectedIndex);
        dst_pt.x += spaceTiling;
        pos++;
    }
}

int Interface::HeroesBar::getSelectedIndex() const
{
    Player &player = *Settings::Get().GetPlayers().GetCurrent();
    Focus &focus = player.GetFocus();
    auto *selectedHero = focus.GetHeroes();

    if (!selectedHero)
        return -1;
    int selected = 0;
    for (Heroes *&hero:this->kingdomHeroes)
    {
        if (hero->GetName() == selectedHero->GetName())
        {
            break;
        }
        selected++;
    }
    return selected;
}

int GetChunkSize(float size1, float size2)
{
    int res = 1;
    if (size1 > size2)
    {
        double intpart;
        double fractpart = modf(size1 / size2, &intpart);
        res = static_cast<int>(intpart);

        if (static_cast<int>(fractpart * 100) > 10)
            res += 1;
    }

    return res;
}

/* redraw HeroesBar cursor */
void Interface::HeroesBar::RedrawCursor()
{
    const Settings &conf = Settings::Get();

}

bool Interface::HeroesBar::EventProcessing()
{
    GameArea &gamearea = interface.GetGameArea();
    Settings &conf = Settings::Get();
    LocalEvent &le = LocalEvent::Get();

    Display &display = Display::Get();
    const Rect &area = Rect(
        ::posLeftSpacing, display.h()-110, 
        display.w()-50, ::spaceTiling);

    // move border
    if (conf.ShowRadar() &&
        BorderWindow::QueueEventProcessing())
    {
        RedrawCursor();
        return false;
    }
    // move cursor
    if (!le.MouseCursor(area))
        return false;


    auto& kingdom =world.GetKingdom(Settings::Get().CurrentColor());
    if(!kingdom.isControlHuman())
        return false;
    SetListContent(kingdom.GetHeroes());
    if (le.MouseClickLeft() || le.MousePressLeft())
    {
        const Point prev(gamearea.GetRectMaps());
        const Point &pt = le.GetMouseCursor();
        int index = (pt.x- posLeftSpacing) / spaceTiling;

        if (area & pt)
        {
            if(index<kingdomHeroes.size())
            {
                Heroes *heroClick = kingdomHeroes[index];
                interface.SetFocus(heroClick);
            }
        }

    }
    return true;
}

void Interface::HeroesBar::ResetAreaSize()
{
    ChangeAreaSize(Size(RADARWIDTH, RADARWIDTH));
}

void Interface::HeroesBar::ChangeAreaSize(const Size &newSize)
{
    if (newSize == area) return;
    const Rect &rect = GetRect();
    Cursor::Get().Hide();
    SetPosition(rect.x < 0 ? 0 : rect.x, rect.y < 0 ? 0 : rect.y, newSize.w, newSize.h);
    Generate();
    RedrawCursor();
    interface.GetGameArea().SetRedraw();
}

void Interface::HeroesBar::SetHeroes(Interface::HEROES *heroes, int count)
{
}

void Interface::HeroesBar::SetListContent(KingdomHeroes &heroes)
{
    kingdomHeroes = heroes;

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

void Interface::HeroesBar::PortraitRedraw(s16 x, s16 y, Heroes& hero, Display &display, bool isFocused)
{
    Surface srfPortrait = hero.GetPortrait(PORT_BIG);

    rectangleFill(srfPortrait, 5, hero.GetLevel(), isFocused, Point(x,y));


    //textPlus.Blit(xText, , srfPortrait);
    //hero.PortraitRedraw(x, y, PORT_BIG, display);
}
