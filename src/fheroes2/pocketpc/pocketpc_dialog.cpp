/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov                               *
 *   afletdinov@mail.dc.baikal.ru                                          *
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
#include "cursor.h"
#include "battle.h"
#include "battle_board.h"
#include "game.h"
#include "text.h"
#include "pocketpc.h"
#include "localevent.h"
#include "icn.h"

u32 PocketPC::GetCursorAttackDialog(const Point &dst, int allow)
{
    Display &display = Display::Get();
    LocalEvent &le = LocalEvent::Get();

    const Rect rt(dst.x - 32, dst.y - 32, 86, 86);

    const Sprite &sp_info = AGG::GetICN(ICN::CMSECO, 5);
    const Sprite &sp_bleft = AGG::GetICN(ICN::CMSECO, 10);
    const Sprite &sp_left = AGG::GetICN(ICN::CMSECO, 11);
    const Sprite &sp_tleft = AGG::GetICN(ICN::CMSECO, 12);
    const Sprite &sp_tright = AGG::GetICN(ICN::CMSECO, 7);
    const Sprite &sp_right = AGG::GetICN(ICN::CMSECO, 8);
    const Sprite &sp_bright = AGG::GetICN(ICN::CMSECO, 9);

    Surface shadow(rt, false);
    shadow.Fill(ColorBlack);

    SpriteBack back(rt);

    Cursor &cursor = Cursor::Get();
    cursor.Hide();
    cursor.SetThemes(Cursor::POINTER);

    // blit alpha
    shadow.SetAlphaMod(120);
    shadow.Blit(rt.x, rt.y, display);

    const Rect rt_info(rt.x + (rt.w - sp_info.w()) / 2, rt.y + (rt.h - sp_info.h()) / 2, sp_info.w(), sp_info.h());
    sp_info.Blit(rt_info.x, rt_info.y);

    const Rect rt_tright(rt.x + 1, rt.y + rt.h - 1 - sp_tright.h(), sp_tright.w(), sp_tright.h());
    if (allow & Battle::BOTTOM_LEFT) sp_tright.Blit(rt_tright.x, rt_tright.y);

    const Rect rt_right(rt.x + 1, rt.y + (rt.h - sp_right.h()) / 2, sp_right.w(), sp_right.h());
    if (allow & Battle::LEFT) sp_right.Blit(rt_right.x, rt_right.y);

    const Rect rt_bright(rt.x + 1, rt.y + 1, sp_bright.w(), sp_bright.h());
    if (allow & Battle::TOP_LEFT) sp_bright.Blit(rt_bright.x, rt_bright.y);

    const Rect rt_tleft(rt.x + rt.w - 1 - sp_tleft.w(), rt.y + rt.h - 1 - sp_tleft.h(), sp_tleft.w(), sp_tleft.h());
    if (allow & Battle::BOTTOM_RIGHT) sp_tleft.Blit(rt_tleft.x, rt_tleft.y);

    const Rect rt_left(rt.x + rt.w - 1 - sp_left.w(), rt.y + (rt.h - sp_left.h()) / 2, sp_left.w(), sp_left.h());
    if (allow & Battle::RIGHT) sp_left.Blit(rt_left.x, rt_left.y);

    const Rect rt_bleft(rt.x + rt.w - 1 - sp_bleft.w(), rt.y + 1, sp_bleft.w(), sp_bleft.h());
    if (allow & Battle::TOP_RIGHT) sp_bleft.Blit(rt_bleft.x, rt_bleft.y);

    cursor.Show();
    display.Flip();

    while (le.HandleEvents() && !le.MouseClickLeft());

    if ((allow & Battle::BOTTOM_LEFT) && (rt_tright & le.GetMouseCursor())) return Cursor::SWORD_TOPRIGHT;
    else if ((allow & Battle::LEFT) && (rt_right & le.GetMouseCursor())) return Cursor::SWORD_RIGHT;
    else if ((allow & Battle::TOP_LEFT) && (rt_bright & le.GetMouseCursor())) return Cursor::SWORD_BOTTOMRIGHT;
    else if ((allow & Battle::BOTTOM_RIGHT) && (rt_tleft & le.GetMouseCursor())) return Cursor::SWORD_TOPLEFT;
    else if ((allow & Battle::RIGHT) && (rt_left & le.GetMouseCursor())) return Cursor::SWORD_LEFT;
    else if ((allow & Battle::TOP_RIGHT) && (rt_bleft & le.GetMouseCursor())) return Cursor::SWORD_BOTTOMLEFT;

    return Cursor::WAR_INFO;
}

Surface CreateTouchButton()
{
    Surface sf(Size(24, 24), false);

    const u32 ww = sf.w() / 2;
    const u32 hh = sf.h() / 2;

    const Sprite &sp0 = AGG::GetICN(ICN::LOCATORS, 22);

    sp0.Blit(Rect(0, 0, ww, hh), Point(0, 0), sf);
    sp0.Blit(Rect(sp0.w() - ww, 0, ww, hh), Point(ww, 0), sf);
    sp0.Blit(Rect(0, sp0.h() - hh, ww, hh), Point(0, hh), sf);
    sp0.Blit(Rect(sp0.w() - ww, sp0.h() - hh, ww, hh), Point(ww, hh), sf);

    return sf;
}

void RedrawTouchButton(const Surface &sf, const Rect &rt, const char *lb)
{
    Display &display = Display::Get();

    if (sf.w() != rt.w)
    {
        const u32 ww = 4;
        sf.Blit(Rect(0, 0, ww, sf.h()), rt.x, rt.y, display);

        if (rt.w > 8)
        {
            const u32 count = (rt.w - ww) / ww;
            for (u32 ii = 0; ii < count; ++ii)
                sf.Blit(Rect(ww, 0, ww, sf.h()), rt.x + ww * (ii + 1), rt.y, display);
        }

        sf.Blit(Rect(sf.w() - ww, 0, ww, sf.h()), rt.x + rt.w - ww, rt.y, display);
    } else
        sf.Blit(rt.x, rt.y, display);

    if (lb)
    {
        Text tx(lb, Font::BIG);
        tx.Blit(rt.x + (rt.w - tx.w()) / 2, rt.y + (rt.h - tx.h()) / 2);
    }
}