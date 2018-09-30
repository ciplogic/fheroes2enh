//
// Created by ciprian on 1/15/18.
//

#include "plus_sign_addon.h"


#include "ColorTable.h"
#include "text.h"
#include "sprite.h"

void PlusSignAddon::draw(int rightMost, int top, bool isEnabled)
{
    Text textPlus = {"+"};
    const Rect pos(rightMost - 14, top + 3, 10, 10);
    Surface greenUp(Size(textPlus.w() + 4, textPlus.h() + 4), false);
    greenUp.SetAlphaMod(120);
    if (isEnabled)
        greenUp.Fill(ColorsTable::Green);
    else
        greenUp.Fill(ColorsTable::Gray);

    const Rect area{0, 0, greenUp.w() - 1, greenUp.h() - 1};
    greenUp.DrawRect(area, RGBA(255, 255, 0));

    const Point ptPlus(pos.x + pos.w - greenUp.w() - 1, pos.y + 2);
    greenUp.Blit(ptPlus.x, ptPlus.y, Display::Get());
    textPlus.Blit(ptPlus.x + 2, ptPlus.y + 1, Display::Get());
}

void PaintArea(Surface& surface, Rect area, RGBA col)
{
    uint32_t iCol = col.pack();
    int offX = area.x;
    int offY = area.y;
    for (int y = 0; y < area.h; y++)
        for (int x = 0; x < area.w; x++)
        {
            surface.SetPixel4(offX+x, offY+y, iCol);
        }
}

namespace
{
    Surface qwikheroC[3];
    Sprite* boxInfoCPtr[3];
    bool Initialize()
    {
        PlusSignAddon::GenerateInfoArea(qwikheroC[0], 191, 189);
        boxInfoCPtr[0] = new Sprite(qwikheroC[0], 0, 0);
        PlusSignAddon::GenerateInfoArea(qwikheroC[1], 158, 64);
        boxInfoCPtr[1] = new Sprite(qwikheroC[1], 0, 0);
        PlusSignAddon::GenerateInfoArea(qwikheroC[2], 223, 185);
        boxInfoCPtr[2] = new Sprite(qwikheroC[2], 0, 0);
        return true;
    }


    bool initialize = Initialize();
}

void PlusSignAddon::GenerateInfoArea(Surface& surface, int w, int h)
{
    surface.Set(w, h, false);
     
    const RGBA brown(30,30,30);
    const RGBA brownBright(255,255,255);
    
    const Rect area{ 0, 0, w, h};
    PaintArea(surface, area, brownBright);

    const Rect area2{ 4, 4, w - 9, h - 9 };
    PaintArea(surface, area2, brown);
}

Sprite& PlusSignAddon::DefaultBackground(int id)
{
    return *boxInfoCPtr[id];
}
