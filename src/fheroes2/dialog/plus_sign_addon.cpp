//
// Created by ciprian on 1/15/18.
//

#include "plus_sign_addon.h"


#include "ColorTable.h"
#include "text.h"

void PlusSignAddon::draw(int rightMost, int top, bool isEnabled)
{
    Text textPlus = {"+"};
    Rect pos(rightMost - 14, top + 3, 10, 10);
    Surface greenUp(Size(textPlus.w() + 4, textPlus.h() + 4), false);
    greenUp.SetAlphaMod(120);
    if (isEnabled)
        greenUp.Fill(ColorsTable::Green);
    else
        greenUp.Fill(ColorsTable::Gray);

    const Point ptPlus(pos.x + pos.w - greenUp.w() - 1, pos.y + 2);
    greenUp.Blit(ptPlus.x, ptPlus.y, Display::Get());
    textPlus.Blit(ptPlus.x + 2, ptPlus.y + 1, Display::Get());

}
