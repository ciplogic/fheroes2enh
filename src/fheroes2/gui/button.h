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

#include "gamedefs.h"
#include "surface.h"

class Button : public Rect
{
public:
    Button();

    Button(s32, s32, int, uint32_t index1, uint32_t index2);

    bool isEnable() const;

    bool isDisable() const;

    bool isPressed() const;

    bool isReleased() const;

    void Press();

    void Release();

    void SetPos(s32, s32);

    void SetPos(const Point &);

    void SetSize(uint32_t, uint32_t);

    void SetSprite(int icn, uint32_t, uint32_t);

    void SetSprite(const Surface &, const Surface &);

    void SetDisable(bool);

    void Draw() const;

    void PressDraw();

    void ReleaseDraw();

protected:
    Surface sf1;
    Surface sf2;

    uint32_t flags;
};

class ButtonSprite : public Button
{
public:
    ButtonSprite() = default;

protected:
    Surface sf;
};

class ButtonGroups
{
public:
    ButtonGroups(Rect, uint32_t);


    void Draw() const;

    int QueueEventProcessing() const;

    void DisableButton1(bool) const;

    void DisableButton2(bool) const;

protected:
    std::unique_ptr<Button> button1;
    std::unique_ptr<Button> button2;
    int result1;
    int result2;
    int buttons;
};


