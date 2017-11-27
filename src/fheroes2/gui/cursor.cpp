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
#include "cursor.h"

/* constructor */
Cursor::Cursor() : theme(NONE), offset_x(0), offset_y(0)
{
}

Cursor &Cursor::Get()
{
    static Cursor _cursor;
    return _cursor;
}

/* get theme cursor */
int Cursor::Themes()
{
    return SP_ARROW >= theme ? theme : NONE;
}

/* set cursor theme */
bool Cursor::SetThemes(int name, bool force)
{
    if (force || theme != name)
    {
        if (isVisible()) Hide();
        theme = name;

        switch (0xF000 & name)
        {
            case 0x3000:
                Set(AGG::GetICN(ICN::SPELCO, 0xFF & name), true);
                DEBUG(DBG_ENGINE, DBG_TRACE, ICN::GetString(ICN::SPELCO) << ", " << (name & 0xFF));
                break;

            case 0x2000:
                Set(AGG::GetICN(ICN::CMSECO, 0xFF & name), true);
                DEBUG(DBG_ENGINE, DBG_TRACE, ICN::GetString(ICN::CMSECO) << ", " << (name & 0xFF));
                break;

            case 0x1000:
                Set(AGG::GetICN(ICN::ADVMCO, 0xFF & name), true);
                DEBUG(DBG_ENGINE, DBG_TRACE, ICN::GetString(ICN::ADVMCO) << ", " << (name & 0xFF));
                break;

            default:
                // default Cursor::POINTER
                Set(AGG::GetICN(ICN::ADVMCO, 0), true);
                break;
        }

        SetOffset(name);
        return true;
    }

    return false;
}

/* redraw cursor wrapper for local event */
void Cursor::Redraw(s32 x, s32 y)
{
    Cursor &cur = Get();

    if (cur.isVisible())
    {
        cur.Move(x, y);

        Display::Get().Flip();
    }
}

/* move cursor */
void Cursor::Move(s32 x, s32 y)
{
    if (isVisible()) SpriteMove::Move(x + offset_x, y + offset_y);
}

/* set offset big cursor */
void Cursor::SetOffset(int name)
{
    switch (name)
    {
        case MOVE:
        case MOVE2:
        case MOVE3:
        case MOVE4:
            offset_x = -12;
            offset_y = -8;
            break;

        case ACTION:
        case ACTION2:
        case ACTION3:
        case ACTION4:
            offset_x = -14;
            offset_y = -10;
            break;

        case BOAT:
        case BOAT2:
        case BOAT3:
        case BOAT4:
        case REDBOAT:
        case REDBOAT2:
        case REDBOAT3:
        case REDBOAT4:
            offset_x = -12;
            offset_y = -12;
            break;

        case CASTLE:
            offset_x = -6;
            offset_y = -4;
            break;

        case SCROLL_TOPRIGHT:
        case SCROLL_RIGHT:
            offset_x = -15;
            offset_y = 0;
            break;

        case SCROLL_BOTTOM:
        case SCROLL_BOTTOMLEFT:
            offset_x = 0;
            offset_y = -15;
            break;

        case SCROLL_BOTTOMRIGHT:
        case SWORD_BOTTOMRIGHT:
            offset_x = -20;
            offset_y = -20;
            break;

        case SWORD_BOTTOMLEFT:
            offset_x = -5;
            offset_y = -20;
            break;

        case SWORD_TOPLEFT:
            offset_x = -5;
            offset_y = -5;
            break;

        case SWORD_TOPRIGHT:
            offset_x = -20;
            offset_y = -5;
            break;

        case SWORD_LEFT:
            offset_x = -5;
            offset_y = -7;
            break;

        case SWORD_RIGHT:
            offset_x = -25;
            offset_y = -7;
            break;

        case WAR_MOVE:
        case WAR_FLY:
            offset_x = -7;
            offset_y = -14;
            break;

        case WAR_NONE:
        case WAR_HERO:
        case WAR_ARROW:
        case WAR_INFO:
        case WAR_BROKENARROW:
            offset_x = -7;
            offset_y = -7;
            break;

        case SP_SLOW:
        case SP_UNKNOWN:
        case SP_CURSE:
        case SP_LIGHTNINGBOLT:
        case SP_CHAINLIGHTNING:
        case SP_CURE:
        case SP_BLESS:
        case SP_FIREBALL:
        case SP_FIREBLAST:
        case SP_TELEPORT:
        case SP_ELEMENTALSTORM:
        case SP_RESURRECT:
        case SP_RESURRECTTRUE:
        case SP_HASTE:
        case SP_SHIELD:
        case SP_ARMAGEDDON:
        case SP_ANTIMAGIC:
        case SP_DISPEL:
        case SP_BERSERKER:
        case SP_PARALYZE:
        case SP_BLIND:
        case SP_HOLYWORD:
        case SP_HOLYSHOUT:
        case SP_METEORSHOWER:
        case SP_ANIMATEDEAD:
        case SP_MIRRORIMAGE:
        case SP_BLOODLUST:
        case SP_DEATHRIPPLE:
        case SP_DEATHWAVE:
        case SP_STEELSKIN:
        case SP_STONESKIN:
        case SP_DRAGONSLAYER:
        case SP_EARTHQUAKE:
        case SP_DISRUPTINGRAY:
        case SP_COLDRING:
        case SP_COLDRAY:
        case SP_HYPNOTIZE:
        case SP_ARROW:
        {
            const Sprite &spr = AGG::GetICN(ICN::SPELCO, 0xFF & name);
            offset_x = -spr.w() / 2;
            offset_y = -spr.h() / 2;
            break;
        }

        default:
            offset_x = 0;
            offset_y = 0;
            break;
    }
}

void Cursor::Show()
{
    if (!Settings::Get().ExtPocketHideCursor()) SpriteMove::Show();
}

int Cursor::DistanceThemes(int theme, u32 dist)
{
    if (0 == dist) return POINTER;
    else if (dist > 4) dist = 4;

    switch (theme)
    {
        case MOVE:
        case FIGHT:
        case BOAT:
        case ANCHOR:
        case CHANGE:
        case ACTION:
            return theme + 6 * (dist - 1);

        case REDBOAT:
            return REDBOAT + dist - 1;

        default:
            break;
    }

    return theme;
}

int Cursor::WithoutDistanceThemes(int theme)
{
    switch (theme)
    {
        case MOVE2:
        case MOVE3:
        case MOVE4:
            return MOVE;
        case FIGHT2:
        case FIGHT3:
        case FIGHT4:
            return FIGHT;
        case BOAT2:
        case BOAT3:
        case BOAT4:
            return BOAT;
        case ANCHOR2:
        case ANCHOR3:
        case ANCHOR4:
            return ANCHOR;
        case CHANGE2:
        case CHANGE3:
        case CHANGE4:
            return CHANGE;
        case ACTION2:
        case ACTION3:
        case ACTION4:
            return ACTION;
        case REDBOAT2:
        case REDBOAT3:
        case REDBOAT4:
            return REDBOAT;

        default:
            break;
    }

    return theme;
}
