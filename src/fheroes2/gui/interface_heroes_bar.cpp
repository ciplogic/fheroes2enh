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
#include "ground.h"
#include "world.h"
#include "icn.h"


#define RADARCOLOR    0x40    // index palette
#define COLOR_DESERT    0x70
#define COLOR_SNOW    0x0A
#define COLOR_SWAMP    0xA0
#define COLOR_WASTELAND    0xD6
#define COLOR_BEACH    0xC6
#define COLOR_LAVA    0x19
#define COLOR_DIRT    0x30
#define COLOR_GRASS    0x60
#define COLOR_WATER    0xF0
#define COLOR_ROAD    0x7A

#define COLOR_BLUE    0x47
#define COLOR_GREEN    0x67
#define COLOR_RED    0xbd
#define COLOR_YELLOW    0x70
#define COLOR_ORANGE    0xcd
#define COLOR_PURPLE    0x87
#define COLOR_GRAY    0x10

u32 GetPaletteIndexFromGround(int ground)
{
    switch (ground)
    {
        case Maps::Ground::DESERT:
            return (COLOR_DESERT);
        case Maps::Ground::SNOW:
            return (COLOR_SNOW);
        case Maps::Ground::SWAMP:
            return (COLOR_SWAMP);
        case Maps::Ground::WASTELAND:
            return (COLOR_WASTELAND);
        case Maps::Ground::BEACH:
            return (COLOR_GRASS);
        case Maps::Ground::WATER:
            return (COLOR_BEACH);
        case Maps::Ground::LAVA:
            return (COLOR_LAVA);
        case Maps::Ground::DIRT:
            return (COLOR_DIRT);
        case Maps::Ground::GRASS:
            return (COLOR_WATER);
        default:
            break;
    }

    return 0;
}

u32 GetPaletteIndexFromColor(int color)
{
    switch (color)
    {
        case Color::BLUE:
            return COLOR_BLUE;
        case Color::GREEN:
            return COLOR_GREEN;
        case Color::RED:
            return COLOR_RED;
        case Color::YELLOW:
            return COLOR_YELLOW;
        case Color::ORANGE:
            return COLOR_ORANGE;
        case Color::PURPLE:
            return COLOR_PURPLE;
        default:
            break;
    }

    return COLOR_GRAY;
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
    Display &display = Display::Get();

    SetPos(30, display.h() - 110);

    const Settings &conf = Settings::Get();
    const Rect &area = GetArea();


    // portrait
    Point dst_pt;
    dst_pt.x = area.x;
    dst_pt.y = area.y;
    const Rect portPos(dst_pt.x, dst_pt.y, 101, 93);
    auto &kingdom = world.GetKingdom(Settings::Get().CurrentColor());
    if (!kingdom.isControlHuman())
        return;
    SetListContent(kingdom.GetHeroes());

    this->_selectedIndex = getSelectedIndex();
    int pos = 0;
    for (Heroes *&hero:kingdomHeroes)
    {
        PortraitRedraw(dst_pt.x, dst_pt.y, *hero, display, pos == _selectedIndex);
        dst_pt.x += 120;
        pos++;
    }
}

int Interface::HeroesBar::getSelectedIndex() const
{
    Player &player = *Settings::Get().GetPlayers().GetCurrent();
    Focus &focus = player.GetFocus();
    auto *selectedHero = focus.GetHeroes();

    int selected = -1;
    if (selectedHero)
    {
        selected = 0;
        for (Heroes *&hero:this->kingdomHeroes)
        {
            if (hero->GetName() == selectedHero->GetName())
            {
                break;
            }
            selected++;
        }
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

void Interface::HeroesBar::QueueEventProcessing()
{
    GameArea &gamearea = interface.GetGameArea();
    Settings &conf = Settings::Get();
    LocalEvent &le = LocalEvent::Get();

    Display &display = Display::Get();
    const Rect &area = Rect(30, display.h()-110, display.w()-50, 120);

    // move border
    if (conf.ShowRadar() &&
        BorderWindow::QueueEventProcessing())
    {
        RedrawCursor();
        return;
    }
    // move cursor
    if (!le.MouseCursor(area))
        return;


    auto& kingdom =world.GetKingdom(Settings::Get().CurrentColor());
    if(!kingdom.isControlHuman())
        return;
    SetListContent(kingdom.GetHeroes());
    if (le.MouseClickLeft() || le.MousePressLeft())
    {
        const Point prev(gamearea.GetRectMaps());
        const Point &pt = le.GetMouseCursor();
        int index = (pt.x-30) / 120;

        if (area & pt)
        {
            if(index<kingdomHeroes.size())
            {
                Heroes *heroClick = kingdomHeroes[index];
                interface.SetFocus(heroClick);
            }
        }
        return;
    }
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

void Interface::HeroesBar::PortraitRedraw(s16 x, s16 y, Heroes& hero, Display &display, bool isFocused)
{
    Surface srfPortrait = hero.GetPortrait(PORT_BIG);
    if(!isFocused)
    {
        srfPortrait.SetAlphaMod(120);
    }
    srfPortrait.Blit(x,y, display);

    //hero.PortraitRedraw(x, y, PORT_BIG, display);
}
