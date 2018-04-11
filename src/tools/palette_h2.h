#pragma once

#include <vector>
#include <SDL_video.h>
#include "types.h"

void fillPalette(std::vector<SDL_Color> &pal_colors, std::vector<uint32_t> &pal_colors_u32);
