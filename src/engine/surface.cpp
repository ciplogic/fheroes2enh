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
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <memory>
#include "surface.h"
#include "error.h"
#include "system.h"

#ifdef WITH_IMAGE
#include "SDL_image.h"
#include "IMG_savepng.h"
#endif

namespace
{
    u32 default_depth = 32;
    RGBA default_color_key;
    SDL_Color *pal_colors = nullptr;
    u32 pal_nums = 0;
}

SurfaceFormat GetRGBAMask(u32 bpp)
{
    SurfaceFormat fm;
    fm.depth = bpp;
    fm.ckey = default_color_key;

    switch (bpp)
    {
        case 32:
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            fm.rmask = 0xff000000;
            fm.gmask = 0x00ff0000;
            fm.bmask = 0x0000ff00;
            fm.amask = 0x000000ff;
#else
            fm.rmask = 0x000000ff;
            fm.gmask = 0x0000ff00;
            fm.bmask = 0x00ff0000;
            fm.amask = 0xff000000;
#endif
            break;

        case 24:
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            fm.rmask = 0x00fc0000;
            fm.gmask = 0x0003f000;
            fm.bmask = 0x00000fc0;
            fm.amask = 0x0000003f;
#else
            fm.rmask = 0x0000003f;
            fm.gmask = 0x00000fc0;
            fm.bmask = 0x0003f000;
            fm.amask = 0x00fc0000;
#endif
            break;

        case 16:
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            fm.rmask = 0x0000f000;
            fm.gmask = 0x00000f00;
            fm.bmask = 0x000000f0;
            fm.amask = 0x0000000f;
#else
            fm.rmask = 0x0000000f;
            fm.gmask = 0x000000f0;
            fm.bmask = 0x00000f00;
            fm.amask = 0x0000f000;
#endif
            break;

        default:
            fm.rmask = 0;
            fm.gmask = 0;
            fm.bmask = 0;
            fm.amask = 0;
            break;
    }
    return fm;
}

u32 GetPixel24(const u8 *ptr)
{
    u32 color = 0;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    color |= ptr[0];
    color <<= 8;
    color |= ptr[1];
    color <<= 8;
    color |= ptr[2];
#else
    color |= ptr[2];
    color <<= 8;
    color |= ptr[1];
    color <<= 8;
    color |= ptr[0];
#endif
    return color;
}

void SetPixel24(u8 *ptr, u32 color)
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    ptr[2] = color;
    ptr[1] = color >> 8;
    ptr[0] = color >> 16;
#else
    ptr[0] = color;
    ptr[1] = color >> 8;
    ptr[2] = color >> 16;
#endif
}

RGBA::RGBA()
{
    color = 0;
}

RGBA::RGBA(int r, int g, int b, int a) noexcept
{
    color = ((r << 24) & 0xFF000000) |
            ((g << 16) & 0x00FF0000) |
            ((b << 8) & 0x0000FF00) |
            (a & 0x000000FF);
}

int RGBA::r() const
{
    int red = (color >> 24) & 0x000000FF;
    return red;
}

int RGBA::g() const
{
    int green = (color >> 16) & 0x000000FF;
    return green;
}

int RGBA::b() const
{
    int blue = (color >> 8) & 0x000000FF;
    return blue;
}

int RGBA::a() const
{
    int alpha = color & 0x000000FF;
    return alpha;
}

SDL_Color RGBA::packSdlColor() const
{
    SDL_Color resColor;
    resColor.r = r();
    resColor.g = g();
    resColor.b = b();
    resColor.unused = a();
    return resColor;
}

u32 RGBA::pack() const
{
    return (((r() << 24) & 0xFF000000) |
            ((g() << 16) & 0x00FF0000) |
            ((b() << 8) & 0x0000FF00) |
            (a() & 0x000000FF));
}

RGBA RGBA::unpack(int v)
{
    int r = (v >> 24) & 0x000000FF;
    int g = (v >> 16) & 0x000000FF;
    int b = (v >> 8) & 0x000000FF;
    int a = v & 0x000000FF;

    return RGBA(r, g, b, a);
}

Surface::Surface() 
: surface(nullptr)
{
}

Surface::Surface(const Size &sz, bool amask) 
: surface(nullptr)
{
    Set(sz.w, sz.h, amask);
}

Surface::Surface(const Size &sz, const SurfaceFormat &fm) : surface(nullptr)
{
    Set(sz.w, sz.h, fm);
}

Surface::Surface(const Surface &bs) : surface(nullptr)
{
    Set(bs, false);
}

Surface::Surface(const std::string &file) : surface(nullptr)
{
    Load(file);
}

Surface::Surface(SDL_Surface *sf) : surface(nullptr)
{
    Set(sf);
}

Surface::Surface(const void *pixels, u32 width, u32 height, u32 bytes_per_pixel /* 1, 2, 3, 4 */, bool amask)
: surface(nullptr)
{
    SurfaceFormat fm = GetRGBAMask(8 * bytes_per_pixel);

    if (8 == fm.depth)
    {

        surface = SDL_CreateRGBSurface(SDL_HWSURFACE, width, height, fm.depth, fm.rmask, fm.gmask, fm.bmask,
                                       amask ? fm.amask : 0);
    	SetPalette();
        Lock();
        memcpy(surface->pixels, pixels, width * height);
        Unlock();
    } else
    {
        surface = SDL_CreateRGBSurfaceFrom(const_cast<void *>(pixels), width, height, fm.depth, width * bytes_per_pixel,
                                           fm.rmask, fm.gmask, fm.bmask, amask ? fm.amask : 0);
    }
}

Surface::~Surface()
{
    if (!surface)
        return;
    FreeSurface(*this);
    surface = nullptr;
}

/* operator = */
Surface &Surface::operator=(const Surface &bs)
{
    Set(bs, false);
    return *this;
}

bool Surface::operator==(const Surface &bs) const
{
    return surface && bs.surface ? surface == bs.surface : false;
}

void Surface::Reset()
{
    FreeSurface(*this);
    surface = nullptr; /* hard set: for ref copy */
}

void Surface::Set(SDL_Surface *sf)
{
    FreeSurface(*this);
    surface = sf;
}

void Surface::Set(const Surface &bs, bool refcopy)
{
    FreeSurface(*this);

    if (bs.isValid())
    {
        surface = SDL_ConvertSurface(bs.surface, bs.surface->format, bs.surface->flags);

        if (!surface)
            Error::Except(__FUNCTION__, SDL_GetError());
        
    }
}

void Surface::Set(u32 sw, u32 sh, bool amask)
{
    Set(sw, sh, default_depth, amask);
}

void Surface::Set(u32 sw, u32 sh, u32 bpp /* bpp: 8, 16, 24, 32 */, bool amask)
{
    if (bpp == 8)
        bpp = 32;

    SurfaceFormat fm = GetRGBAMask(bpp);

    if (8 == fm.depth || !amask) fm.amask = 0;
    Set(sw, sh, fm);
}

void Surface::Set(u32 sw, u32 sh, const SurfaceFormat &fm)
{
    FreeSurface(*this);

    surface = SDL_CreateRGBSurface(SDL_HWSURFACE, sw, sh, fm.depth, fm.rmask, fm.gmask, fm.bmask, fm.amask);

    if (!surface)
        Error::Except(__FUNCTION__, SDL_GetError());

    if (8 == depth())
    {
        SetPalette();
        Fill(fm.ckey);
        SetColorKey(fm.ckey);
    } else if (amask())
    {
        Fill(RGBA(fm.ckey.r(), fm.ckey.g(), fm.ckey.b(), 0));
        SetColorKey(fm.ckey);
    } else if (fm.ckey.pack())
    {
        Fill(fm.ckey);
        SetColorKey(fm.ckey);
    }

    if (amask())
    {
        SDL_SetAlpha(surface, SDL_SRCALPHA, 255);
    } else
    {
        SDL_SetAlpha(surface, 0, 0);
    }
}

void Surface::SetDefaultPalette(SDL_Color *ptr, int num)
{
    pal_colors = ptr;
    pal_nums = num;
}

void Surface::SetDefaultColorKey(int r, int g, int b)
{
    default_color_key = RGBA(r, g, b);
}

void Surface::SetDefaultDepth(u32 depth)
{
    switch (depth)
    {
        case 24:
        ERROR("switch to 32 bpp colors");
            default_depth = 32;
            break;

        case 8:
        case 15:
        case 16:
            default_depth = depth;
            break;

        case 32:
            default_depth = depth;
            break;

        default:
            break;
    }
}

Size Surface::GetSize() const
{
    return Size(w(), h());
}

bool Surface::isValid() const
{
    return surface && surface->format;
}

bool Surface::Load(const std::string &fn)
{
    FreeSurface(*this);

    surface = IMG_Load(fn.c_str());

    if (!surface)
    ERROR(SDL_GetError());

    return surface;
}

bool Surface::Save(const std::string &fn) const
{
    int res = IMG_SavePNG(fn.c_str(), surface, -1);

    if (0 != res)
    {
        ERROR(SDL_GetError());
        return false;
    }

    return true;
}

int Surface::w() const
{
    return surface ? surface->w : 0;
}

int Surface::h() const
{
    return surface ? surface->h : 0;
}

u32 Surface::depth() const
{
    return isValid() ? surface->format->BitsPerPixel : 0;
}

u32 Surface::amask() const
{
    return isValid() ? surface->format->Amask : 0;
}

u32 Surface::alpha() const
{
    return isValid() ? surface->format->alpha : 0;
}

SurfaceFormat Surface::GetFormat() const
{
    SurfaceFormat res;
    if (surface->format)
    {
        res.depth = surface->format->BitsPerPixel;
        res.rmask = surface->format->Rmask;
        res.gmask = surface->format->Gmask;
        res.bmask = surface->format->Bmask;
        res.amask = surface->format->Amask;
        res.ckey = default_color_key;
    }
    return res;
}

u32 Surface::MapRGB(const RGBA &color) const
{
    return amask() ? SDL_MapRGBA(surface->format, color.r(), color.g(), color.b(), color.a()) : SDL_MapRGB(
            surface->format, color.r(), color.g(), color.b());
}

RGBA Surface::GetRGB(u32 pixel) const
{
    u8 r = 0;
    u8 g = 0;
    u8 b = 0;
    u8 a = 0;

    if (amask())
    {
        SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);
        return RGBA(r, g, b, a);
    }

    SDL_GetRGB(pixel, surface->format, &r, &g, &b);
    return RGBA(r, g, b);
}

/* load static palette (economize 1kb for each surface) only 8bit color! */
void Surface::SetPalette()
{
    if (isValid() &&
        pal_colors && pal_nums && surface->format->palette)
    {
#ifndef WIN32
		if (surface->format->palette->colors &&
			pal_colors != surface->format->palette->colors)
			SDL_free(surface->format->palette->colors);
#endif
        surface->format->palette->colors = pal_colors;
        surface->format->palette->ncolors = pal_nums;
    }
}

u32 Surface::GetColorKey() const
{
    return isValid() && (surface->flags & SDL_SRCCOLORKEY) ? surface->format->colorkey : 0;
}

void Surface::SetColorKey(const RGBA &color)
{
    SDL_SetColorKey(surface, SDL_SRCCOLORKEY, MapRGB(color));
}

/* draw u32 pixel */
void Surface::SetPixel4(s32 x, s32 y, u32 color) const
{
    u32 *bufp = static_cast<u32 *>(surface->pixels) + y * (surface->pitch >> 2) + x;
    *bufp = color;
}

/* draw u24 pixel */
void Surface::SetPixel3(s32 x, s32 y, u32 color) const
{
    u8 *bufp = static_cast<u8 *>(surface->pixels) + y * surface->pitch + x * 3;
    SetPixel24(bufp, color);
}

/* draw u16 pixel */
void Surface::SetPixel2(s32 x, s32 y, u32 color) const
{
    u16 *bufp = static_cast<u16 *>(surface->pixels) + y * (surface->pitch >> 1) + x;
    *bufp = static_cast<u16>(color);
}

/* draw u8 pixel */
void Surface::SetPixel1(s32 x, s32 y, u32 color) const
{
    u8 *bufp = static_cast<u8 *>(surface->pixels) + y * surface->pitch + x;
    *bufp = static_cast<u8>(color);
}

/* draw pixel */

void Surface::SetPixel(int x, int y, u32 pixel) const
{
    if (x < w() && y < h())
    {
        switch (depth())
        {
            case 8:
                SetPixel1(x, y, pixel);
                break;
            case 15:
            case 16:
                SetPixel2(x, y, pixel);
                break;
            case 24:
                SetPixel3(x, y, pixel);
                break;
            case 32:
                SetPixel4(x, y, pixel);
                break;
            default:
                break;
        }
    } else
    {
        std::ostringstream os;
        os << "out of range: " << "x: " << x << ", " << "y: " << y << ", " << "width: " << w() << ", " << "height: "
           << h();
        Error::Except(__FUNCTION__, os.str().c_str());
    }
}

u32 Surface::GetPixel4(s32 x, s32 y) const
{
    u32 *bufp = static_cast<u32 *>(surface->pixels) + y * (surface->pitch >> 2) + x;
    return *bufp;
}

u32 Surface::GetPixel3(s32 x, s32 y) const
{
    u8 *bufp = static_cast<u8 *>(surface->pixels) + y * surface->pitch + x * 3;
    return GetPixel24(bufp);
}

u32 Surface::GetPixel2(s32 x, s32 y) const
{
    u16 *bufp = static_cast<u16 *>(surface->pixels) + y * (surface->pitch >> 1) + x;
    return static_cast<u32>(*bufp);
}

u32 Surface::GetPixel1(s32 x, s32 y) const
{
    u8 *bufp = static_cast<u8 *>(surface->pixels) + y * surface->pitch + x;
    return static_cast<u32>(*bufp);
}

u32 Surface::GetPixel(int x, int y) const
{
    u32 pixel = 0;

    if (x < w() && y < h())
    {
        switch (depth())
        {
            case 8:
                pixel = GetPixel1(x, y);
                break;
            case 15:
            case 16:
                pixel = GetPixel2(x, y);
                break;
            case 24:
                pixel = GetPixel3(x, y);
                break;
            case 32:
                pixel = GetPixel4(x, y);
                break;
            default:
                break;
        }
    } else
        Error::Except(__FUNCTION__, "out of range");

    return pixel;
}
void Surface::BlitAlpha(const Rect &srt, const Point &dpt, Surface &dst) const
{
    auto w = srt.w, h = srt.h;
    for(int x = 0; x<w;x++){
        for(int y = 0; y<h;y++){
            u32 srcPix = this->GetPixel4(srt.x + x, srt.y+y);
            u32 alpha = srcPix>>24;
            if(alpha==0)
                continue;

            if(alpha==255)
            {
                dst.SetPixel4(dpt.x+x, dpt.y + y, srcPix);
                continue;
            }
            u32 dstPix = dst.GetPixel4(dpt.x+x, dpt.y + y);
            u32 dstRed = dstPix & 0xff;
            u32 dstGreen = (dstPix>>8) & 0xff;
            u32 dstBlue = (dstPix>>16) & 0xff;
            u32 srcRed = srcPix & 0xff;
            u32 srcGreen = (srcPix>>8) & 0xff;
            u32 srcBlue = (srcPix>>16) & 0xff;
            u32 opacity = alpha ;
            u32 revOpacity = 255 - opacity;
            u32 finalRed = (opacity*srcRed + revOpacity*dstRed)>>8;
            if(finalRed>255) finalRed = 255;
            u32 finalGreen =(opacity*srcGreen + revOpacity*dstGreen)>>8;
            if(finalGreen>255) finalGreen = 255;
            u32 finalBlue = (opacity*srcBlue + revOpacity*dstBlue)>>8;
            if(finalBlue>255) finalBlue = 255;

            u32 finalPix = 0xff000000 | (finalBlue<<16) | (finalGreen<<8) | (finalRed);
            dst.SetPixel4(dpt.x+x, dpt.y + y, finalPix);

        }
    }
}

void Surface::Blit(const Rect &srt, const Point &dpt, Surface &dst) const
{
	SDL_Rect dstrect;
	SDLRect(dpt.x, dpt.y, srt.w, srt.h, dstrect);
	SDL_Rect srcrect;
	SDLRect(srt, srcrect);

    if (amask() && dst.amask())
    {
        SDL_SetAlpha(surface, 0, 0);
        SDL_BlitSurface(surface, &srcrect, dst.surface, &dstrect);
        SDL_SetAlpha(surface, SDL_SRCALPHA, 255);
    } else
        SDL_BlitSurface(surface, &srcrect, dst.surface, &dstrect);
}

void Surface::Blit(Surface &dst) const
{
    Blit(Rect(Point(0, 0), GetSize()), Point(0, 0), dst);
}

void Surface::Blit(s32 dx, s32 dy, Surface &dst) const
{
    Blit(Point(dx, dy), dst);
}

void Surface::Blit(const Rect &srt, s32 dx, s32 dy, Surface &dst) const
{
    Blit(srt, Point(dx, dy), dst);
}

void Surface::Blit(const Point &dpt, Surface &dst) const
{
    Blit(Rect(Point(0, 0), GetSize()), dpt, dst);
}

void Surface::SetAlphaMod(int level)
{
    if (isValid())
    {
        if (amask())
        {
            Surface res(GetSize(), false);
            SDL_SetAlpha(surface, 0, 0);
            Blit(res);
            SDL_SetAlpha(surface, SDL_SRCALPHA, 255);
            Set(res, true);
        }

        SDL_SetAlpha(surface, SDL_SRCALPHA, level);
    }
}

void Surface::Lock() const
{
    if (SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);
}

void Surface::Unlock() const
{
    if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
}

bool Surface::isRefCopy() const
{
    return surface != nullptr && 1 < surface->refcount;
}

void Surface::FreeSurface(Surface &sf)
{
    if (!sf.surface)
		return;

	// clear static palette
	if (sf.surface->format && 8 == sf.surface->format->BitsPerPixel && pal_colors && pal_nums &&
		sf.surface->format->palette && pal_colors == sf.surface->format->palette->colors)
	{
		sf.surface->format->palette->colors = nullptr;
		sf.surface->format->palette->ncolors = 0;
	}

	SDL_FreeSurface(sf.surface);
	sf.surface = nullptr;
}

u32 Surface::GetMemoryUsage() const
{
    u32 res = sizeof(surface);

    if (!surface)
	{
		return res;
	}
	res += sizeof(SDL_Surface) + sizeof(SDL_PixelFormat) + surface->pitch * surface->h;

	if (surface->format && surface->format->palette &&
		(!pal_colors || pal_colors != surface->format->palette->colors))
		res += sizeof(SDL_Palette) + surface->format->palette->ncolors * sizeof(SDL_Color);

	return res;
}

std::string Surface::Info() const
{
    std::ostringstream os;

    if (isValid())
    {
        os <<
           "flags" << "(" << surface->flags << ", " << (surface->flags & SDL_SRCALPHA ? "SRCALPHA" : "")
           << (surface->flags & SDL_SRCCOLORKEY ? "SRCCOLORKEY" : "") << "), " <<
           "w" << "(" << surface->w << "), " <<
           "h" << "(" << surface->h << "), " <<
           "size" << "(" << GetMemoryUsage() << "), " <<
           "bpp" << "(" << depth() << "), " <<
           "Amask" << "(" << "0x" << std::setw(8) << std::setfill('0') << std::hex << surface->format->Amask << "), " <<
           "colorkey" << "(" << "0x" << std::setw(8) << std::setfill('0') << surface->format->colorkey << "), "
           << std::dec <<
           "alpha" << "(" << alpha() << "), ";
    } else
        os << "invalid surface";

    return os.str();
}

void Surface::Swap(Surface &sf1, Surface &sf2)
{
    std::swap(sf1.surface, sf2.surface);
}

Surface Surface::RenderScale(const Size &size) const
{
    Surface res(size, GetFormat());

    if (size.w >= 2 && size.h >= 2)
    {
        float stretch_factor_x = size.w / static_cast<float>(w());
        float stretch_factor_y = size.h / static_cast<float>(h());

        res.Lock();
        for (s32 yy = 0; yy < h(); yy++)
            for (s32 xx = 0; xx < w(); xx++)
                for (s32 oy = 0; oy < stretch_factor_y; ++oy)
                    for (s32 ox = 0; ox < stretch_factor_x; ++ox)
                    {
                        res.SetPixel(static_cast<s32>(stretch_factor_x * xx) + ox,
                                     static_cast<s32>(stretch_factor_y * yy) + oy, GetPixel(xx, yy));
                    }
        res.Unlock();
    }

    return res;
}

Surface Surface::RenderReflect(int shape /* 0: none, 1 : vert, 2: horz, 3: both */) const
{
    Surface res(GetSize(), GetFormat());

    switch (shape % 4)
    {
        // normal
        default:
            Blit(res);
            break;

            // vertical reflect
        case 1:
            res.Lock();
            for (int yy = 0; yy < h(); ++yy)
                for (int xx = 0; xx < w(); ++xx)
                    res.SetPixel(xx, h() - yy - 1, GetPixel(xx, yy));
            res.Unlock();
            break;

            // horizontal reflect
        case 2:
            res.Lock();
            for (int yy = 0; yy < h(); ++yy)
                for (int xx = 0; xx < w(); ++xx)
                    res.SetPixel(w() - xx - 1, yy, GetPixel(xx, yy));
            res.Unlock();
            break;

            // both variants
        case 3:
            res.Lock();
            for (int yy = 0; yy < h(); ++yy)
                for (int xx = 0; xx < w(); ++xx)
                    res.SetPixel(w() - xx - 1, h() - yy - 1, GetPixel(xx, yy));
            res.Unlock();
            break;
    }
    return res;
}

Surface Surface::RenderRotate(int parm /* 0: none, 1 : 90 CW, 2: 90 CCW, 3: 180 */) const
{
    // 90 CW or 90 CCW
    if (parm == 1 || parm == 2)
    {
        Surface res(Size(h(), w()), GetFormat()); /* height <-> width */

        res.Lock();
        for (int yy = 0; yy < h(); ++yy)
            for (int xx = 0; xx < w(); ++xx)
            {
                if (parm == 1)
                    res.SetPixel(yy, w() - xx - 1, GetPixel(xx, yy));
                else
                    res.SetPixel(h() - yy - 1, xx, GetPixel(xx, yy));
            }
        res.Unlock();
        return res;
    } 
	if (parm == 3)
        return RenderReflect(3);

    return RenderReflect(0);
}

Surface Surface::RenderStencil(const RGBA &color) const
{
    Surface res(GetSize(), GetFormat());
    u32 clkey0 = GetColorKey();
    RGBA clkey = GetRGB(clkey0);
    const u32 pixel = res.MapRGB(color);

    res.Lock();
    for (int y = 0; y < h(); ++y)
        for (int x = 0; x < w(); ++x)
        {
            RGBA col = GetRGB(GetPixel(x, y));
            if ((clkey0 && clkey == col) || col.a() < 200) continue;
            res.SetPixel(x, y, pixel);
        }
    res.Unlock();
    return res;
}

Surface Surface::RenderContour(const RGBA &color) const
{
    const RGBA fake = RGBA(0x00, 0xFF, 0xFF);
    Surface res(GetSize(), GetFormat());
    Surface trf = RenderStencil(fake);
    u32 clkey0 = trf.GetColorKey();
    RGBA clkey = trf.GetRGB(clkey0);
    const u32 pixel = res.MapRGB(color);
    const u32 fake2 = trf.MapRGB(fake);

    res.Lock();
    for (int y = 0; y < trf.h(); ++y)
		for (int x = 0; x < trf.w(); ++x)
		{
			if (fake2 != trf.GetPixel(x, y))
				continue;
			if (0 == x || 0 == y ||
			trf.w() - 1 == x || trf.h() - 1 == y)
			{
				res.SetPixel(x, y, pixel);
				continue;
			}
			if (0 < x)
			{
				RGBA col = trf.GetRGB(trf.GetPixel(x - 1, y));
				if ((clkey0 && col == clkey) || col.a() < 200) res.SetPixel(x - 1, y, pixel);
			}
			if (trf.w() - 1 > x)
			{
				RGBA col = trf.GetRGB(trf.GetPixel(x + 1, y));
				if ((clkey0 && col == clkey) || col.a() < 200) res.SetPixel(x + 1, y, pixel);
			}

			if (0 < y)
			{
				RGBA col = trf.GetRGB(trf.GetPixel(x, y - 1));
				if ((clkey0 && col == clkey) || col.a() < 200) res.SetPixel(x, y - 1, pixel);
			}
			if (trf.h() - 1 > y)
			{
				RGBA col = trf.GetRGB(trf.GetPixel(x, y + 1));
				if ((clkey0 && col == clkey) || col.a() < 200) res.SetPixel(x, y + 1, pixel);
			}
		}
    res.Unlock();
    return res;
}

Surface Surface::RenderGrayScale() const
{
    Surface res(GetSize(), GetFormat());
    const u32 colkey = GetColorKey();

    res.Lock();
    for (int y = 0; y < h(); ++y)
        for (int x = 0; x < w(); ++x)
        {
            u32 pixel = GetPixel(x, y);
            if (0 == colkey || pixel != colkey)
            {
                RGBA col = GetRGB(pixel);
                int z = col.r() * 0.299f + col.g() * 0.587f + col.b() * 0.114f;
                pixel = res.MapRGB(RGBA(z, z, z, col.a()));
                res.SetPixel(x, y, pixel);
            }
        }
    res.Unlock();
    return res;
}

Surface Surface::RenderSepia() const
{
    Surface res(GetSize(), GetFormat());
    const u32 colkey = GetColorKey();

    res.Lock();
    for (int x = 0; x < w(); x++)
        for (int y = 0; y < h(); y++)
        {
            u32 pixel = GetPixel(x, y);
            if (colkey == 0 || pixel != colkey)
            {
                RGBA col = GetRGB(pixel);
                //Numbers derived from http://blogs.techrepublic.com.com/howdoi/?p=120
#define CLAMP255(val) std::min<u16>((val), 255)
                int outR = CLAMP255(static_cast<u16>(col.r() * 0.693f + col.g() * 0.769f + col.b() * 0.189f));
                int outG = CLAMP255(static_cast<u16>(col.r() * 0.449f + col.g() * 0.686f + col.b() * 0.168f));
                int outB = CLAMP255(static_cast<u16>(col.r() * 0.272f + col.g() * 0.534f + col.b() * 0.131f));
                pixel = res.MapRGB(RGBA(outR, outG, outB, col.a()));
                res.SetPixel(x, y, pixel);
#undef CLAMP255
            }
        }
    res.Unlock();
    return res;
}

Surface Surface::RenderChangeColor(const RGBA &col1, const RGBA &col2) const
{
    Surface res = GetSurface();
    u32 fc = MapRGB(col1);
    u32 tc = res.MapRGB(col2);

    if (amask())
        fc |= amask();

    if (res.amask())
        tc |= res.amask();

    res.Lock();
    if (fc != tc)
        for (int y = 0; y < h(); ++y)
            for (int x = 0; x < w(); ++x)
                if (fc == GetPixel(x, y)) res.SetPixel(x, y, tc);
    res.Unlock();
    return res;
}

Surface Surface::GetSurface() const
{
    return GetSurface(Rect(Point(0, 0), GetSize()));
}

Surface Surface::GetSurface(const Rect &rt) const
{
    SurfaceFormat fm = GetFormat();
	
    Surface res(rt, fm);

    if (amask())
        SDL_SetAlpha(surface, 0, 0);

    Blit(rt, Point(0, 0), res);

    if (amask())
        SDL_SetAlpha(surface, SDL_SRCALPHA, 255);

    return res;
}

void Surface::Fill(const RGBA &col)
{
    FillRect(Rect(0, 0, w(), h()), col);
}

void Surface::FillRect(const Rect &rect, const RGBA &col)
{
	SDL_Rect dstrect;
	SDLRect(rect, dstrect);
    SDL_FillRect(surface, &dstrect, MapRGB(col));
}

// swaps two numbers
void swap(int* a , int*b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

// returns absolute value of number
float absolute(float x )
{
    if (x < 0) return -x;
	return x;
}

//returns integer part of a floating point number
int iPartOfNumber(float x)
{
    return static_cast<int>(x);
}

//rounds off a number
int roundNumber(float x)
{
    return iPartOfNumber(x + 0.5) ;
}

//returns fractional part of a number
float fPartOfNumber(float x)
{
    if (x>0) return x - iPartOfNumber(x);
    else return x - (iPartOfNumber(x)+1);

}

//returns 1 - fractional part of number
float rfPartOfNumber(float x)
{
    return 1 - fPartOfNumber(x);
}

// draws a pixel on screen of given brightness
// 0<=brightness<=1. We can use your own library
// to draw on screen
void Surface::drawPixel( int x , int y , float brightness, const u32 col)
{
    int finalColA = 255*brightness;
    if(finalColA>255)
        finalColA = 255;

    u32 uCol = (col & 0xffffff)+(finalColA<<24);
    SetPixel4(x,y,uCol);
}

void Surface::drawAALine(int x0 , int y0 , int x1 , int y1, const RGBA& col)
{
    int steep = absolute(y1 - y0) > absolute(x1 - x0) ;

    const u32 uCol = MapRGB(col);

    // swap the co-ordinates if slope > 1 or we
    // draw backwards
    if (steep)
    {
        swap(&x0 , &y0);
        swap(&x1 , &y1);
    }
    if (x0 > x1)
    {
        swap(&x0 ,&x1);
        swap(&y0 ,&y1);
    }

    //compute the slope
    float dx = x1-x0;
    float dy = y1-y0;
    float gradient = dy/dx;
    if (dx == 0.0)
        gradient = 1;

    int xpxl1 = x0;
    int xpxl2 = x1;
    float intersectY = y0;

    // main loop
    if (steep)
    {
	    for (int x = xpxl1 ; x <=xpxl2 ; x++)
        {
            // pixel coverage is determined by fractional
            // part of y co-ordinate
            int y = iPartOfNumber(intersectY);
            drawPixel(y, x,
                      rfPartOfNumber(intersectY), uCol);
            drawPixel(y-1, x,
                      fPartOfNumber(intersectY), uCol);
            intersectY += gradient;
        }
    }
    else
    {
	    for (int x = xpxl1 ; x <=xpxl2 ; x++)
        {
            // pixel coverage is determined by fractional
            // part of y co-ordinate
            int y = iPartOfNumber(intersectY);
            drawPixel(x, y,
                      rfPartOfNumber(intersectY), uCol);
            drawPixel(x, y-1,
                      fPartOfNumber(intersectY), uCol);
            intersectY += gradient;
        }
    }

}

void Surface::DrawLineAa(const Point &p1, const Point &p2, const RGBA &color)
{
    int x1 = p1.x;
    int y1 = p1.y;
    int x2 = p2.x;
    int y2 = p2.y;

    const int dx = abs(x2 - x1);
    const int dy = abs(y2 - y1);

    Lock();
    drawAALine(x1, y1, x2,y2, color);
    Unlock();
}
void Surface::DrawLine(const Point &p1, const Point &p2, const RGBA &color)
{
    int x1 = p1.x;
    int y1 = p1.y;
    int x2 = p2.x;
    int y2 = p2.y;

    const u32 pixel = MapRGB(color);
    const int dx = abs(x2 - x1);
    const int dy = abs(y2 - y1);

    Lock();
    if (dx > dy)
    {
        int ns = div(dx, 2).quot;

        for (int i = 0; i <= dx; ++i)
        {
            SetPixel(x1, y1, pixel);
            x1 < x2 ? ++x1 : --x1;
            ns -= dy;
            if (ns < 0)
            {
                y1 < y2 ? ++y1 : --y1;
                ns += dx;
            }
        }
    } else
    {
        int ns = div(dy, 2).quot;

        for (int i = 0; i <= dy; ++i)
        {
            SetPixel(x1, y1, pixel);
            y1 < y2 ? ++y1 : --y1;
            ns -= dx;
            if (ns < 0)
            {
                x1 < x2 ? ++x1 : --x1;
                ns += dy;
            }
        }
    }
    Unlock();
}

void Surface::DrawPoint(const Point &pt, const RGBA &color) const
{
    Lock();
    SetPixel(pt.x, pt.y, MapRGB(color));
    Unlock();
}

void Surface::DrawRect(const Rect &rt, const RGBA &color) const
{
    const u32 pixel = MapRGB(color);

    Lock();
    for (int i = rt.x; i < rt.x + rt.w; ++i)
    {
        SetPixel(i, rt.y, pixel);
        SetPixel(i, rt.y + rt.y + rt.h - 1, pixel);
    }

    for (int i = rt.y; i < rt.y + rt.h; ++i)
    {
        SetPixel(rt.x, i, pixel);
        SetPixel(rt.x + rt.w - 1, i, pixel);
    }
    Unlock();
}

void Surface::DrawBorder(const RGBA &color, bool solid)
{
    if (solid)
        DrawRect(Rect(Point(0, 0), GetSize()), color);
    else
    {
        const u32 pixel = MapRGB(color);

        for (int i = 0; i < w(); ++i)
        {
            SetPixel(i, 0, pixel);
            if (i + 1 < w()) SetPixel(i + 1, 0, pixel);
            i += 3;
        }
        for (int i = 0; i < w(); ++i)
        {
            SetPixel(i, h() - 1, pixel);
            if (i + 1 < w()) SetPixel(i + 1, h() - 1, pixel);
            i += 3;
        }
        for (int i = 0; i < h(); ++i)
        {
            SetPixel(0, i, pixel);
            if (i + 1 < h()) SetPixel(0, i + 1, pixel);
            i += 3;
        }
        for (int i = 0; i < h(); ++i)
        {
            SetPixel(w() - 1, i, pixel);
            if (i + 1 < h()) SetPixel(w() - 1, i + 1, pixel);
            i += 3;
        }
    }
}

Surface Surface::RenderSurface(const Size &sz) const
{
    return RenderSurface(Rect(Point(0, 0), GetSize()), sz);
}

Surface Surface::RenderSurface(const Rect &srcrt, const Size &sz) const
{
    const Surface &srcsf = *this;
    Surface dstsf(sz, false);
    Rect dstrt = Rect(0, 0, sz.w, sz.h);
    u32 mw = dstrt.w < srcrt.w ? dstrt.w : srcrt.w;
    u32 mh = dstrt.h < srcrt.h ? dstrt.h : srcrt.h;

    u32 cw = mw / 3;
    u32 ch = mh / 3;
    s32 cx = srcrt.x + (srcrt.w - cw) / 2;
    s32 cy = srcrt.y + (srcrt.h - ch) / 2;
    u32 bw = mw - 2 * cw;
    u32 bh = mh - 2 * ch;

    u32 ox = (dstrt.w - (dstrt.w / bw) * bw) / 2;
    u32 oy = (dstrt.h - (dstrt.h / bh) * bh) / 2;

    // body
    if (bw < dstrt.w && bh < dstrt.h)
        for (u32 yy = 0; yy < (dstrt.h / bh); ++yy)
            for (u32 xx = 0; xx < (dstrt.w / bw); ++xx)
                srcsf.Blit(Rect(cx, cy, bw, bh), dstrt.x + ox + xx * bw, dstrt.y + oy + yy * bh, dstsf);

    // top, bottom bar
    for (u32 xx = 0; xx < (dstrt.w / bw); ++xx)
    {
        s32 dstx = dstrt.x + ox + xx * bw;
        srcsf.Blit(Rect(cx, srcrt.y, bw, ch), dstx, dstrt.y, dstsf);
        srcsf.Blit(Rect(cx, srcrt.y + srcrt.h - ch, bw, ch), dstx, dstrt.y + dstrt.h - ch, dstsf);
    }

    // left, right bar
    for (u32 yy = 0; yy < (dstrt.h / bh); ++yy)
    {
        s32 dsty = dstrt.y + oy + yy * bh;
        srcsf.Blit(Rect(srcrt.x, cy, cw, bh), dstrt.x, dsty, dstsf);
        srcsf.Blit(Rect(srcrt.x + srcrt.w - cw, cy, cw, bh), dstrt.x + dstrt.w - cw, dsty, dstsf);
    }

    // top left angle
    srcsf.Blit(Rect(srcrt.x, srcrt.y, cw, ch), dstrt.x, dstrt.y, dstsf);

    // top right angle
    srcsf.Blit(Rect(srcrt.x + srcrt.w - cw, srcrt.y, cw, ch), dstrt.x + dstrt.w - cw, dstrt.y, dstsf);

    // bottom left angle
    srcsf.Blit(Rect(srcrt.x, srcrt.y + srcrt.h - ch, cw, ch), dstrt.x, dstrt.y + dstrt.h - ch, dstsf);

    // bottom right angle
    srcsf.Blit(Rect(srcrt.x + srcrt.w - cw, srcrt.y + srcrt.h - ch, cw, ch), dstrt.x + dstrt.w - cw,
               dstrt.y + dstrt.h - ch, dstsf);

    return dstsf;
}
