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
#include <cctype>
#include "agg.h"
#include "settings.h"
#include "text.h"
#include "system.h"
#include "tools.h"

TextInterface::TextInterface(int ft) : font(ft)
{
}

TextAscii::TextAscii(const string& msg, int ft) : TextInterface(ft), message(msg)
{
}

void TextAscii::SetText(const string& msg)
{
    message = msg;
}

void TextAscii::SetFont(int ft)
{
    const Settings& conf = Settings::Get();
    font = ft;
}

void TextAscii::Clear()
{
    message.clear();
}

size_t TextAscii::Size() const
{
    return message.size();
}

int TextAscii::CharWidth(int c, int f)
{
    return c < 0x21 ? (Font::SMALL == f || Font::YELLOW_SMALL == f ? 4 : 6) : AGG::GetLetter(c, f).w();
}

int TextAscii::CharHeight(int f)
{
    return CharAscent(f) + CharDescent(f) + 1;
}

int TextAscii::CharAscent(int f)
{
    return Font::SMALL == f || Font::YELLOW_SMALL == f ? 8 : 13;
}

int TextAscii::CharDescent(int f)
{
    return Font::SMALL == f || Font::YELLOW_SMALL == f ? 2 : 3;
}

int TextAscii::w(uint32_t s, uint32_t c) const
{
    uint32_t res = 0;
    uint32_t size = message.size();

    if (size)
    {
        if (s > size - 1) s = size - 1;
        if (!c || c > size) c = size - s;

        for (uint32_t ii = s; ii < s + c; ++ii)
            res += CharWidth(message[ii], font);
    }

    return res;
}

int TextAscii::w() const
{
    return w(0, message.size());
}

int TextAscii::h() const
{
    return h(0);
}

int TextAscii::h(int width) const
{
    if (message.empty()) return 0;
    if (0 == width || w() <= width) return CharHeight(font);

    int res = 0;
    int www = 0;

    string::const_iterator pos1 = message.begin();
    string::const_iterator pos2 = message.end();
    string::const_iterator space = pos2;

    while (pos1 < pos2)
    {
        if (isspace(*pos1)) space = pos1;

        if (www + CharWidth(*pos1, font) >= width)
        {
            www = 0;
            res += CharHeight(font);
            if (pos2 != space) pos1 = space + 1;
            space = pos2;
            continue;
        }

        www += CharWidth(*pos1, font);
        ++pos1;
    }

    return res;
}

void TextAscii::Blit(s32 ax, s32 ay, int maxw, Surface& dst)
{
    if (message.empty()) return;

    int oy = 0;
    int sx = ax;

    for (string::const_iterator
         it = message.begin(); it != message.end(); ++it)
    {
        if (maxw && ax - sx >= maxw) break;

        // space or unknown letter
        if (*it < 0x21)
        {
            ax += CharWidth(*it, font);
            continue;
        }

        const Surface& sprite = AGG::GetLetter(*it, font);
        if (!sprite.isValid()) return;

        // valign
        switch (*it)
        {
        case '-':
            oy = CharAscent(font) / 2;
            break;

        case '_':
            oy = CharAscent(font);
            break;

            // "
        case 0x22:
            // '
        case 0x27:
            oy = 0;
            break;

        case 'y':
        case 'g':
        case 'p':
        case 'q':
        case 'j':
            oy = CharAscent(font) + CharDescent(font) - sprite.h();
            break;

        default:
            oy = CharAscent(font) - sprite.h();
            break;
        }

        sprite.Blit(ax, ay + 2 + oy, dst);
        ax += sprite.w();
    }
}

#ifdef WITH_TTF

TextUnicode::TextUnicode(const std::string& msg, int ft) : TextInterface(ft), message(StringUTF8_to_UNICODE(msg))
{
}

TextUnicode::TextUnicode(const u16* pt, size_t sz, int ft) : TextInterface(ft), message(pt, pt + sz)
{
}

bool TextUnicode::isspace(int c)
{
    switch (c)
    {
    case 0x0009:
    case 0x000a:
    case 0x000b:
    case 0x000c:
    case 0x000d:
    case 0x0020:
        return true;

    default:
        break;
    }

    return false;
}

void TextUnicode::SetText(const std::string& msg)
{
    message = StringUTF8_to_UNICODE(msg);
}

void TextUnicode::SetFont(int ft)
{
    const Settings& conf = Settings::Get();
    font = ft;
}

void TextUnicode::Clear()
{
    message.clear();
}

size_t TextUnicode::Size() const
{
    return message.size();
}

int TextUnicode::CharWidth(int c, int f)
{
    return c < 0x0021 ? (Font::SMALL == f || Font::YELLOW_SMALL == f ? 4 : 6) : AGG::GetUnicodeLetter(c, f).w();
}

int TextUnicode::CharHeight(int f)
{
    return AGG::GetFontHeight(true) + 2;
}

int TextUnicode::CharAscent(int f)
{
    return 0;
}

int TextUnicode::CharDescent(int f)
{
    return 0;
}

int TextUnicode::w(uint32_t s, uint32_t c) const
{
    uint32_t res = 0;
    uint32_t size = message.size();

    if (size)
    {
        if (s > size - 1) s = size - 1;
        if (!c || c > size) c = size - s;

        for (uint32_t ii = s; ii < s + c; ++ii)
            res += CharWidth(message[ii], font);
    }

    return res;
}

int TextUnicode::w() const
{
    return w(0, message.size());
}

int TextUnicode::h() const
{
    return h(0);
}

int TextUnicode::h(int width) const
{
    if (message.empty()) return 0;

    if (0 == width || w() <= width) return CharHeight(font);

    int res = 0;
    int www = 0;

    auto pos1 = message.begin();
    auto pos2 = message.end();
    auto space = pos2;

    while (pos1 < pos2)
    {
        if (isspace(*pos1)) space = pos1;

        if (www + CharWidth(*pos1, font) >= width)
        {
            www = 0;
            res += CharHeight(font);
            if (pos2 != space) pos1 = space + 1;
            space = pos2;
            continue;
        }

        www += CharWidth(*pos1, font);
        ++pos1;
    }

    return res;
}

void TextUnicode::Blit(s32 ax, s32 ay, int maxw, Surface& dst)
{
    const s32 sx = ax;

    for (auto& it : message)
    {
        if (maxw && ax - sx >= maxw) break;

        // end string
        if (0 == it) continue;

        // space or unknown letter
        if (it < 0x0021)
        {
            ax += CharWidth(it, font);
            continue;
        }

        const Surface& sprite = AGG::GetUnicodeLetter(it, font);
        if (!sprite.isValid()) return;

        const Surface& shaddow = AGG::GetUnicodeLetter(it, font == 4 || font == 2 ? Font::SHADDOW_BIG : Font::SHADDOW);
        shaddow.Blit(ax + 1, ay + 1, dst);
        sprite.Blit(ax, ay, dst);
        ax += sprite.w();
    }
}

#endif


Text::Text() : message(nullptr), gw(0), gh(0)
{
    message = sp<TextInterface>(new TextUnicode());
}

Text::Text(const string& msg, int ft) : message(nullptr), gw(0), gh(0)
{
    message = sp<TextInterface>(new TextUnicode(msg, ft));

    gw = message->w();
    gh = message->h();
}

Text::Text(const u16* pt, size_t sz, int ft) : message(nullptr), gw(0), gh(0)
{
    if (!pt) return;
    message = sp<TextInterface>(new TextUnicode(pt, sz, ft));

    gw = message->w();
    gh = message->h();
}

Text::Text(const Text& t)
{
    message = sp<TextInterface>(new TextUnicode(static_cast<TextUnicode &>(*t.message)));

    gw = t.gw;
    gh = t.gh;
}

Text& Text::operator=(const Text& t)
{
    message = sp<TextInterface>(new TextUnicode(static_cast<TextUnicode &>(*t.message)));

    gw = t.gw;
    gh = t.gh;

    return *this;
}

void Text::Set(const string& msg, int ft)
{
    message->SetText(msg);
    message->SetFont(ft);
    gw = message->w();
    gh = message->h();
}

void Text::Set(const string& msg)
{
    message->SetText(msg);
    gw = message->w();
    gh = message->h();
}

void Text::Set(int ft)
{
    message->SetFont(ft);
    gw = message->w();
    gh = message->h();
}

void Text::Clear()
{
    message->Clear();
    gw = 0;
    gh = 0;
}

size_t Text::Size() const
{
    return message->Size();
}

void Text::Blit(const Point& dst_pt, Surface& dst) const
{
    return message->Blit(dst_pt.x, dst_pt.y, 0, dst);
}

void Text::Blit(s32 ax, s32 ay, Surface& dst) const
{
    return message->Blit(ax, ay, 0, dst);
}

void Text::Blit(s32 ax, s32 ay, int maxw, Surface& dst) const
{
    return message->Blit(ax, ay, maxw, dst);
}

uint32_t Text::width(const string& str, int ft, uint32_t start, uint32_t count)
{
    if (Settings::Get().Unicode())
    {
        TextUnicode text(str, ft);
        return text.w(start, count);
    }
    TextAscii text(str, ft);
    return text.w(start, count);
}

uint32_t Text::height(const string& str, int ft, uint32_t width)
{
    if (str.empty())
        return 0;
    if (Settings::Get().Unicode())
    {
        TextUnicode text(str, ft);
        return text.h(width);
    }
    TextAscii text(str, ft);
    return text.h(width);
}

TextBox::TextBox() : align(ALIGN_CENTER)
{
}

TextBox::TextBox(const string& msg, int ft, uint32_t width) : align(ALIGN_CENTER)
{
    Set(msg, ft, width);
}

TextBox::TextBox(const string& msg, int ft, const Rect& rt) : align(ALIGN_CENTER)
{
    Set(msg, ft, rt.w);
    Blit(rt.x, rt.y);
}

void TextBox::Set(const string& msg, int ft, uint32_t width)
{
    messages.clear();
    if (msg.empty()) return;

    if (Settings::Get().Unicode())
    {
        std::vector<u16> unicode = StringUTF8_to_UNICODE(msg);

        const u16 sep = '\n';
        std::vector<u16> substr;
        substr.reserve(msg.size());
        auto pos1 = unicode.begin();
        std::vector<u16>::iterator pos2;
        while (unicode.end() != (pos2 = std::find(pos1, unicode.end(), sep)))
        {
            substr.assign(pos1, pos2);
            Append(substr, ft, width);
            pos1 = pos2 + 1;
        }
        if (pos1 < unicode.end())
        {
            substr.assign(pos1, unicode.end());
            Append(substr, ft, width);
        }
    }
    else
    {
        const char sep = '\n';
        string substr;
        substr.reserve(msg.size());
        string::const_iterator pos1 = msg.begin();
        string::const_iterator pos2;
        while (msg.end() != (pos2 = find(pos1, msg.end(), sep)))
        {
            substr.assign(pos1, pos2);
            Append(substr, ft, width);
            pos1 = pos2 + 1;
        }
        if (pos1 < msg.end())
        {
            substr.assign(pos1, msg.end());
            Append(substr, ft, width);
        }
    }
}

void TextBox::SetAlign(int f)
{
    align = f;
}

void TextBox::Append(const string& msg, int ft, uint32_t width)
{
    const Settings& conf = Settings::Get();
    uint32_t www = 0;
    Rect::w = width;

    string::const_iterator pos1 = msg.begin();
    string::const_iterator pos2 = pos1;
    string::const_iterator pos3 = msg.end();
    string::const_iterator space = pos2;

    while (pos2 < pos3)
    {
        if (isspace(*pos2)) space = pos2;
        int char_w = TextAscii::CharWidth(*pos2, ft);

        if (www + char_w >= width)
        {
            www = 0;
            Rect::h += TextAscii::CharHeight(ft);
            if (pos3 != space) pos2 = space + 1;

            if (pos3 != space)
                messages.emplace_back(msg.substr(pos1 - msg.begin(), pos2 - pos1 - 1), ft);
            else
                messages.emplace_back(msg.substr(pos1 - msg.begin(), pos2 - pos1), ft);

            pos1 = pos2;
            space = pos3;
            continue;
        }

        www += char_w;
        ++pos2;
    }

    if (pos1 != pos2)
    {
        Rect::h += TextAscii::CharHeight(ft);
        messages.emplace_back(msg.substr(pos1 - msg.begin(), pos2 - pos1), ft);
    }
}

void TextBox::Append(const std::vector<u16>& msg, int ft, uint32_t width)
{
    const Settings& conf = Settings::Get();

    uint32_t www = 0;
    Rect::w = width;

    auto pos1 = msg.begin();
    auto pos2 = pos1;
    auto pos3 = msg.end();
    auto space = pos2;

    while (pos2 < pos3)
    {
        if (TextUnicode::isspace(*pos2)) space = pos2;
        uint32_t char_w = TextUnicode::CharWidth(*pos2, ft);

        if (www + char_w >= width)
        {
            www = 0;
            Rect::h += TextUnicode::CharHeight(ft);
            if (pos3 != space) pos2 = space + 1;

            if (pos3 != space)
                messages.emplace_back(&msg.at(pos1 - msg.begin()), pos2 - pos1 - 1, ft);
            else
                messages.emplace_back(&msg.at(pos1 - msg.begin()), pos2 - pos1, ft);

            pos1 = pos2;
            space = pos3;
            continue;
        }

        www += char_w;
        ++pos2;
    }

    if (pos1 != pos2)
    {
        Rect::h += TextUnicode::CharHeight(ft);
        messages.emplace_back(&msg.at(pos1 - msg.begin()), pos2 - pos1, ft);
    }
}

void TextBox::Blit(s32 ax, s32 ay, Surface& sf)
{
    Rect::x = ax;
    Rect::y = ay;

    for (auto& message : messages)
    {
        switch (align)
        {
        case ALIGN_LEFT:
            message.Blit(ax, ay);
            break;

        case ALIGN_RIGHT:
            message.Blit(ax + Rect::w - message.w(), ay);
            break;

            // center
        default:
            message.Blit(ax + (Rect::w - message.w()) / 2, ay);
            break;
        }

        ay += message.h();
    }
}

void TextBox::Blit(const Point& pt, Surface& sf)
{
    Blit(pt.x, pt.y, sf);
}

TextSprite::TextSprite() : hide(true)
{
}

TextSprite::TextSprite(const string& msg, int ft, const Point& pt) : Text(msg, ft), hide(true)
{
    back.Save(Rect(pt, gw, gh + 5));
}

TextSprite::TextSprite(const string& msg, int ft, s32 ax, s32 ay) : Text(msg, ft), hide(true)
{
    back.Save(Rect(ax, ay, gw, gh + 5));
}

void TextSprite::Show()
{
    Blit(back.GetPos());
    hide = false;
}

void TextSprite::Hide()
{
    if (!hide) back.Restore();
    hide = true;
}

void TextSprite::SetText(const string& msg)
{
    Hide();
    Set(msg);
    back.Save(Rect(back.GetPos(), gw, gh + 5));
}

void TextSprite::SetText(const string& msg, int ft)
{
    Hide();
    Set(msg, ft);
    back.Save(Rect(back.GetPos(), gw, gh + 5));
}

void TextSprite::SetFont(int ft)
{
    Hide();
    Set(ft);
    back.Save(Rect(back.GetPos(), gw, gh + 5));
}

void TextSprite::SetPos(s32 ax, s32 ay)
{
    back.Save(Rect(ax, ay, gw, gh + 5));
}

int TextSprite::w() const
{
    return back.GetSize().w;
}

int TextSprite::h() const
{
    return back.GetSize().h;
}

bool TextSprite::isHide() const
{
    return hide;
}

bool TextSprite::isShow() const
{
    return !hide;
}

const Rect& TextSprite::GetRect() const
{
    return back.GetArea();
}
