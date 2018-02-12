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

#include <algorithm>
#include <functional>
#include "agg.h"
#include "text.h"
#include "game.h"
#include "cursor.h"
#include "dialog.h"
#include "heroes_base.h"
#include "dialog_selectitems.h"
#include "settings.h"
#include "icn.h"

#define SPELL_PER_PAGE        6
#define SPELL_PER_PAGE_SMALL    2

struct SpellFiltered : binary_function<Spell, int, bool>
{
    bool operator()(Spell s, int f) const
    {
        return ((SpellBook::ADVN & f) && s.isCombat()) || ((SpellBook::CMBT & f) && !s.isCombat());
    }
};

void SpellBookRedrawLists(const SpellStorage &, Rects &, size_t, const Point &, u32, int only, const HeroBase &hero);

void SpellBookRedrawSpells(const SpellStorage &, Rects &, size_t, s32, s32, const HeroBase &hero);

void SpellBookRedrawMP(const Point &, u32);

bool SpellBookSortingSpell(const Spell &spell1, const Spell &spell2)
{
    return ((spell1.isCombat() != spell2.isCombat() && spell1.isCombat()) ||
            (string(spell1.GetName()) < string(spell2.GetName())));
}

Spell SpellBook::Open(const HeroBase &hero, int filter, bool canselect) const
{
    if (!hero.HaveSpellBook())
    {
        Message("", _("No spell to cast."), Font::BIG, Dialog::OK);
        return Spell(Spell::NONE);
    }

    Display &display = Display::Get();
    Cursor &cursor = Cursor::Get();

    const int oldcursor = cursor.Themes();

    const Sprite &r_list = AGG::GetICN(ICN::BOOK, 0);
    const Sprite &l_list = AGG::GetICN(ICN::BOOK, 0, true);

    int filterLocal = filter;
    SpellStorage spells2 = SetFilter(filterLocal, &hero);

    if (canselect && spells2.empty())
    {
        Message("", _("No spell to cast."), Font::BIG, Dialog::OK);
        return Spell::NONE;
    }

    // sorting results
    sort(spells2.begin(), spells2.end(), SpellBookSortingSpell);

    size_t current_index = 0;

    cursor.Hide();
    cursor.SetThemes(Cursor::POINTER);

    const Sprite &bookmark_info = AGG::GetICN(ICN::BOOK, 6);
    const Sprite &bookmark_advn = AGG::GetICN(ICN::BOOK, 3);
    const Sprite &bookmark_cmbt = AGG::GetICN(ICN::BOOK, 4);
    const Sprite &bookmark_clos = AGG::GetICN(ICN::BOOK, 5);

    const Rect pos((display.w() - (r_list.w() + l_list.w())) / 2, (display.h() - r_list.h()) / 2,
                   r_list.w() + l_list.w(), r_list.h() + 70);
    SpriteBack back(pos);

    const Rect prev_list(pos.x + 30, pos.y + 8, 30, 25);
    const Rect next_list(pos.x + 410, pos.y + 8, 30, 25);

    const Rect info_rt(pos.x + 125, pos.y + 275, bookmark_info.w(), bookmark_info.h());
    const Rect advn_rt(pos.x + 270, pos.y + 270, bookmark_advn.w(), bookmark_advn.h());
    const Rect cmbt_rt(pos.x + 304, pos.y + 278, bookmark_cmbt.w(), bookmark_cmbt.h());
    const Rect clos_rt(pos.x + 420, pos.y + 284, bookmark_clos.w(), bookmark_clos.h());

    Spell curspell(Spell::NONE);

    Rects coords;
    coords.reserve(SPELL_PER_PAGE * 2);

    SpellBookRedrawLists(spells2, coords, current_index, pos, hero.GetSpellPoints(), filter, hero);
    bool redraw = false;

    cursor.Show();
    display.Flip();

    LocalEvent &le = LocalEvent::Get();

    // message loop
    while (le.HandleEvents())
    {
        if (le.MouseClickLeft(prev_list) && current_index)
        {
            current_index -= SPELL_PER_PAGE * 2;
            redraw = true;
        } else if (le.MouseClickLeft(next_list) &&
                   spells2.size() > (current_index + SPELL_PER_PAGE * 2))
        {
            current_index += SPELL_PER_PAGE * 2;
            redraw = true;
        } else if ((le.MouseClickLeft(info_rt)) ||
                   (le.MousePressRight(info_rt)))
        {
            string str = _("Your hero has %{point} spell points remaining");
            StringReplace(str, "%{point}", hero.GetSpellPoints());
            cursor.Hide();
            Message("", str, Font::BIG, Dialog::OK);
            cursor.Show();
            display.Flip();
        } else if (le.MouseClickLeft(advn_rt) && filterLocal != ADVN && filter != CMBT)
        {
            filterLocal = ADVN;
            current_index = 0;
            spells2 = SetFilter(filterLocal, &hero);
            redraw = true;
        } else if (le.MouseClickLeft(cmbt_rt) && filterLocal != CMBT && filter != ADVN)
        {
            filterLocal = CMBT;
            current_index = 0;
            spells2 = SetFilter(filterLocal, &hero);
            redraw = true;
        } else if (le.MouseClickLeft(clos_rt) ||
                   HotKeyPressEvent(Game::EVENT_DEFAULT_EXIT))
            break;
        else if (le.MouseClickLeft(pos))
        {
            const s32 index = coords.GetIndex(le.GetMouseCursor());

            if (0 <= index)
            {
                const_iterator spell = spells2.begin() + (index + current_index);

                if (spell < spells2.end())
                {
                    if (canselect)
                    {
                        string str;
                        if (hero.CanCastSpell(*spell, &str))
                        {
                            curspell = *spell;
                            break;
                        } else
                        {
                            cursor.Hide();
                            StringReplace(str, "%{mana}", (*spell).SpellPoint(&hero));
                            StringReplace(str, "%{point}", hero.GetSpellPoints());
                            Message("", str, Font::BIG, Dialog::OK);
                            cursor.Show();
                            display.Flip();
                        }
                    } else
                    {
                        cursor.Hide();
                        Dialog::SpellInfo(*spell, true);
                        cursor.Show();
                        display.Flip();
                    }
                }
            }
        }

        if (le.MousePressRight(pos))
        {
            const s32 index = coords.GetIndex(le.GetMouseCursor());

            if (0 <= index)
            {
                const_iterator spell = spells2.begin() + (index + current_index);
                if (spell < spells2.end())
                {
                    cursor.Hide();
                    Dialog::SpellInfo(*spell, false);
                    cursor.Show();
                    display.Flip();
                }
            }
        }

        if (redraw)
        {
            cursor.Hide();
            SpellBookRedrawLists(spells2, coords, current_index, pos, hero.GetSpellPoints(), filter, hero);
            cursor.Show();
            display.Flip();
            redraw = false;
        }
    }

    cursor.Hide();
    back.Restore();
    cursor.SetThemes(oldcursor);
    cursor.Show();
    display.Flip();

    return curspell;
}

void SpellBook::Edit(const HeroBase &hero)
{
    Display &display = Display::Get();
    Cursor &cursor = Cursor::Get();

    const int oldcursor = cursor.Themes();

    const Sprite &r_list = AGG::GetICN(ICN::BOOK, 0);
    const Sprite &l_list = AGG::GetICN(ICN::BOOK, 0, true);

    size_t current_index = 0;
    SpellStorage spells2 = SetFilter(ALL, &hero);

    cursor.Hide();
    cursor.SetThemes(Cursor::POINTER);

    const Sprite &bookmark_clos = AGG::GetICN(ICN::BOOK, 5);

    const Rect pos((display.w() - (r_list.w() + l_list.w())) / 2, (display.h() - r_list.h()) / 2,
                   r_list.w() + l_list.w(), r_list.h() + 70);
    SpriteBack back(pos);

    const Rect prev_list(pos.x + 30, pos.y + 8, 30, 25);
    const Rect next_list(pos.x + 410, pos.y + 8, 30, 25);
    const Rect clos_rt(pos.x + 420, pos.y + 284, bookmark_clos.w(), bookmark_clos.h());

    Rects coords;
    coords.reserve(SPELL_PER_PAGE * 2);

    SpellBookRedrawLists(spells2, coords, current_index, pos, hero.GetSpellPoints(), ALL, hero);
    bool redraw = false;

    cursor.Show();
    display.Flip();

    LocalEvent &le = LocalEvent::Get();

    // message loop
    while (le.HandleEvents())
    {
        if (le.MouseClickLeft(prev_list) && current_index)
        {
            current_index -= SPELL_PER_PAGE * 2;
            redraw = true;
        } else if (le.MouseClickLeft(next_list) && size() > (current_index + SPELL_PER_PAGE * 2))
        {
            current_index += SPELL_PER_PAGE * 2;
            redraw = true;
        } else if (le.MouseClickLeft(clos_rt) ||
                   HotKeyPressEvent(Game::EVENT_DEFAULT_EXIT))
            break;
        else if (le.MouseClickLeft(pos))
        {
            const s32 index = coords.GetIndex(le.GetMouseCursor());

            if (0 <= index)
            {
                const_iterator spell = spells2.begin() + (index + current_index);

                if (spell < spells2.end())
                {
                    Dialog::SpellInfo(*spell, true);
                    redraw = true;
                }
            } else
            {
                Spell spell = Dialog::SelectSpell();
                spells2.Append(spell);
                Append(spell);
                redraw = true;
            }
        }

        if (le.MousePressRight(pos))
        {
            const s32 index = coords.GetIndex(le.GetMouseCursor());

            if (0 <= index)
            {
                const_iterator spell = spells2.begin() + (index + current_index);

                if (spell < spells2.end())
                {
                    Dialog::SpellInfo(*spell, false);
                    redraw = true;
                }
            }
        }

        if (redraw)
        {
            cursor.Hide();
            SpellBookRedrawLists(spells2, coords, current_index, pos, hero.GetSpellPoints(), ALL, hero);
            cursor.Show();
            display.Flip();
            redraw = false;
        }
    }

    cursor.Hide();
    back.Restore();
    cursor.SetThemes(oldcursor);
    cursor.Show();
    display.Flip();
}

SpellStorage SpellBook::SetFilter(int filter, const HeroBase *hero) const
{
    SpellStorage res(*this);

    // added heroes spell scrolls
    if (hero) res.Append(hero->GetBagArtifacts());

    if (filter != ALL)
    {
        res.resize(distance(res.begin(),
                            remove_if(res.begin(), res.end(), bind2nd(SpellFiltered(), filter))));
    }

    // check on water: disable portal spells
    if (hero && hero->Modes(Heroes::SHIPMASTER))
    {
        auto itend = res.end();
        itend = remove(res.begin(), itend, Spell(Spell::TOWNGATE));
        itend = remove(res.begin(), itend, Spell(Spell::TOWNPORTAL));
        if (res.end() != itend)
            res.resize(distance(res.begin(), itend));
    }

    return res;
}

void SpellBookRedrawMP(const Point &dst, u32 mp)
{
    Point tp(dst.x + 11, dst.y + 9);
    if (0 == mp)
    {
        Text text("0", Font::SMALL);
        text.Blit(tp.x - text.w() / 2, tp.y);
    } else
        for (u32 i = 100; i >= 1; i /= 10)
            if (mp >= i)
            {
                Text text(Int2Str((mp % (i * 10)) / i), Font::SMALL);
                text.Blit(tp.x - text.w() / 2, tp.y);
                tp.y += 0 + text.h();
            }
}

void
SpellBookRedrawLists(const SpellStorage &spells, Rects &coords, const size_t cur, const Point &pt, u32 sp, int only,
                     const HeroBase &hero)
{
    const Sprite &r_list = AGG::GetICN(ICN::BOOK, 0);
    const Sprite &l_list = AGG::GetICN(ICN::BOOK, 0, true);
    const Sprite &bookmark_info = AGG::GetICN(ICN::BOOK, 6);
    const Sprite &bookmark_advn = AGG::GetICN(ICN::BOOK, 3);
    const Sprite &bookmark_cmbt = AGG::GetICN(ICN::BOOK, 4);
    const Sprite &bookmark_clos = AGG::GetICN(ICN::BOOK, 5);

    const Rect info_rt(pt.x + 125, pt.y + 275, bookmark_info.w(), bookmark_info.h());
    const Rect advn_rt(pt.x + 270, pt.y + 270, bookmark_advn.w(), bookmark_advn.h());
    const Rect cmbt_rt(pt.x + 304, pt.y + 278, bookmark_cmbt.w(), bookmark_cmbt.h());
    const Rect clos_rt(pt.x + 420, pt.y + 284, bookmark_clos.w(), bookmark_clos.h());

    l_list.Blit(pt.x, pt.y);
    r_list.Blit(pt.x + l_list.w(), pt.y);
    bookmark_info.Blit(info_rt);
    if (SpellBook::CMBT != only)
        bookmark_advn.Blit(advn_rt);
    if (SpellBook::ADVN != only)
        bookmark_cmbt.Blit(cmbt_rt);
    bookmark_clos.Blit(clos_rt);

    if (!coords.empty()) coords.clear();

    SpellBookRedrawMP(info_rt, sp);
    SpellBookRedrawSpells(spells, coords, cur, pt.x, pt.y, hero);
    SpellBookRedrawSpells(spells, coords, cur + SPELL_PER_PAGE,
                          pt.x + 220, pt.y, hero);
}

void
SpellBookRedrawSpells(const SpellStorage &spells, Rects &coords, const size_t cur, s32 px, s32 py, const HeroBase &hero)
{

    s32 ox = 0;
    s32 oy = 0;

    for (u32 ii = 0; ii < SPELL_PER_PAGE; ++ii)
        if (spells.size() > cur + ii)
        {
            if (0 == (ii % (SPELL_PER_PAGE / 2)))
            {
                oy = 50;
                ox += 80;
            }

            const Spell &spell = spells[ii + cur];
            const Sprite &icon = AGG::GetICN(ICN::SPELLS, spell.IndexSprite());
            const Rect rect(px + ox - icon.w() / 2, py + oy - icon.h() / 2, icon.w(), icon.h() + 10);

            icon.Blit(rect.x, rect.y);

            // multiple icons for mass spells
            switch (spell())
            {
                case Spell::MASSBLESS:
                case Spell::MASSCURE:
                case Spell::MASSHASTE:
                case Spell::MASSSLOW:
                case Spell::MASSCURSE:
                case Spell::MASSDISPEL:
                case Spell::MASSSHIELD:
                    icon.Blit(rect.x - 10, rect.y + 8);
                    icon.Blit(rect.x + 10, rect.y + 8);
                    break;

                default:
                    break;
            }

            TextBox box(string(spell.GetName()) + " [" + Int2Str(spell.SpellPoint(&hero)) + "]", Font::SMALL,
                        80);
            box.Blit(px + ox - 40, py + oy + 25);

            oy += 80;

            coords.push_back(rect);
        }
}
