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

#include <string>
#include <vector>
#include "gamedefs.h"
#include "display.h"
#include "sprites.h"

namespace Font
{
    enum
    {
        SMALL = 0x01,
        BIG = 0x02,
        YELLOW_BIG = 0x04,
        YELLOW_SMALL = 0x08,
        SHADDOW = 0x10,
        SHADDOW_BIG = 0x20
    };
}

enum
{
    ALIGN_NONE,
    ALIGN_LEFT,
    ALIGN_CENTER,
    ALIGN_RIGHT
};

class TextInterface
{
public:
    TextInterface(int ft = Font::BIG);

    virtual ~TextInterface()
    = default;;

    virtual void SetText(const string&) = 0;

    virtual void SetFont(int) = 0;

    virtual void Clear() = 0;

    virtual int w() const = 0;

    virtual int h() const = 0;

    virtual size_t Size() const = 0;

    virtual void Blit(s32, s32, int maxw, Surface& sf = Display::Get()) = 0;

    int font;
};


class TextAscii : public TextInterface
{
public:
    TextAscii() = default;

    TextAscii(const string&, int = Font::BIG);

    void SetText(const string&);

    void SetFont(int);

    void Clear();

    int w() const;

    int w(uint32_t, uint32_t) const;

    int h() const;

    int h(int) const;

    size_t Size() const;

    void Blit(s32, s32, int maxw, Surface& sf = Display::Get());

    static int CharWidth(int, int ft);

    static int CharHeight(int ft);

    static int CharAscent(int ft);

    static int CharDescent(int ft);

private:
    string message;
};

#ifdef WITH_TTF

class TextUnicode : public TextInterface
{
public:
    TextUnicode() = default;

    TextUnicode(const std::string&, int ft = Font::BIG);

    TextUnicode(const u16*, size_t, int ft = Font::BIG);

    void SetText(const std::string&);

    void SetFont(int);

    void Clear();

    int w() const;

    int w(uint32_t, uint32_t) const;

    int h() const;

    int h(int) const;

    size_t Size() const;

    void Blit(s32, s32, int maxw, Surface& sf = Display::Get());

    static bool isspace(int);

    static int CharWidth(int, int ft);

    static int CharHeight(int ft);

    static int CharAscent(int ft);

    static int CharDescent(int ft);

private:
    std::vector<u16> message;
};

#endif

class Text
{
public:
    Text();

    Text(const string&, int ft = Font::BIG);

#ifdef WITH_TTF

    Text(const u16*, size_t, int ft = Font::BIG);

#endif

    Text(const Text&);

    ~Text() = default;

    Text& operator=(const Text&);

    void Set(const string&, int);

    void Set(const string&);

    void Set(int);

    void Clear();

    size_t Size() const;

    int w() const
    {
        return gw;
    }

    int h() const
    {
        return gh;
    }

    void Blit(s32, s32, Surface& sf = Display::Get()) const;

    void Blit(s32, s32, int maxw, Surface& sf = Display::Get()) const;

    void Blit(const Point&, Surface& sf = Display::Get()) const;

    static uint32_t width(const string&, int ft, uint32_t start = 0, uint32_t count = 0);

    static uint32_t height(const string&, int ft, uint32_t width = 0);

protected:
    sp<TextInterface> message;
    uint32_t gw;
    uint32_t gh;
};

class TextSprite : protected Text
{
public:
    TextSprite();

    TextSprite(const string&, int ft, const Point& pt);

    TextSprite(const string&, int ft, s32, s32);

    void SetPos(const Point& pt)
    {
        SetPos(pt.x, pt.y);
    }

    void SetPos(s32, s32);

    void SetText(const string&);

    void SetText(const string&, int);

    void SetFont(int);

    void Show();

    void Hide();

    bool isHide() const;

    bool isShow() const;

    int w() const;

    int h() const;

    const Rect&
    GetRect() const;

private:
    SpriteBack back;
    bool hide;
};

class TextBox : protected Rect
{
public:
    TextBox();

    TextBox(const string&, int, uint32_t width);

    TextBox(const string&, int, const Rect&);

    void Set(const string&, int, uint32_t width);

    void SetAlign(int type);

    const Rect& GetRect() const
    {
        return *this;
    }

    s32 x() const
    {
        return Rect::x;
    }

    s32 y() const
    {
        return Rect::y;
    }

    int w() const
    {
        return Rect::w;
    }

    int h() const
    {
        return Rect::h;
    }

    uint32_t row() const
    {
        return messages.size();
    }

    void Blit(s32, s32, Surface& sf = Display::Get());

    void Blit(const Point&, Surface& sf = Display::Get());

private:
    void Append(const string&, int, uint32_t);

#ifdef WITH_TTF

    void Append(const std::vector<u16>&, int, uint32_t);

#endif

    vector<Text> messages;
    int align;
};
