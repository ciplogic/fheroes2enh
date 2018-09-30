//
// Created by ciprian on 1/15/18.
//

#pragma once
#include "surface.h"
#include "sprite.h"

class PlusSignAddon
{
public:
    static void draw(int rightMost, int top, bool isEnabled);
    static void GenerateInfoArea(Surface& surface, int w, int h);

    static Sprite& DefaultBackground(int id);
};
