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
#include "rect.h"
#include "types.h"

struct Point;
struct Rect;
struct SDL_Surface;

class RGBA
{
public:
    RGBA();

    constexpr RGBA(int r, int g, int b, int a = 255) 
        : color((r << 24 & 0xFF000000) |
            (g << 16 & 0x00FF0000) |
            (b << 8 & 0x0000FF00) |
            (a & 0x000000FF)){
    }

    bool operator==(const RGBA &col) const
    { return pack() == col.pack(); }

    bool operator!=(const RGBA &col) const
    { return pack() != col.pack(); }

    int r() const;

    int g() const;

    int b() const;

    int a() const;

    uint32_t pack() const;

    SDL_Color packSdlColor() const;

    static RGBA unpack(int);

    static uint32_t packRgba(uint32_t r, uint32_t g, uint32_t b, uint32_t a = 255)
    {
        return r + (g << 8) + (b << 16) + (a << 24);
    }

protected:
    uint32_t color;
};


#define ColorBlack RGBA(0,0,0,255)

struct SurfaceFormat
{
    uint32_t depth;
    uint32_t rmask;
    uint32_t gmask;
    uint32_t bmask;
    uint32_t amask;
    RGBA ckey;

    SurfaceFormat() : depth(0), rmask(0), gmask(0), bmask(0), amask(0)
    {}
};

class Surface
{
public:
    Surface();

    Surface(const Size &, bool amask);

    Surface(const Size &, const SurfaceFormat &);

    explicit Surface(const std::string &);

    Surface(const void *pixels, uint32_t width, uint32_t height, uint32_t bytes_per_pixel /* 1, 2, 3, 4 */,
            bool amask);  /* agg: create raw tile */
    Surface(const Surface &);

    explicit Surface(SDL_Surface *);

    Surface &operator=(const Surface &);

    bool operator==(const Surface &) const;

    SDL_Surface *operator()() const
    { return surface; }

    virtual ~Surface();

    void Set(uint32_t sw, uint32_t sh, const SurfaceFormat &);

    void Set(uint32_t sw, uint32_t sh, bool amask);

    virtual void Reset();

    bool Load(const std::string &);

    bool Save(const std::string &) const;

    int w() const;

    int h() const;

    uint32_t depth() const;

    uint32_t amask() const;

    uint32_t alpha() const;

    Size GetSize() const;

    bool isRefCopy() const;

    SurfaceFormat GetFormat() const;

    bool isValid() const;

    void SetColorKey(const RGBA &);

    uint32_t GetColorKey() const;

    void BlitAlpha(const Rect &srt, const Point &dpt, Surface &dst) const;

    void Blit(Surface &) const;

    void Blit(s32, s32, Surface &) const;

    void Blit(const Point &, Surface &) const;

    void Blit(const Rect &srt, s32, s32, Surface &) const;

    void Blit(const Rect &srt, const Point &, Surface &) const;

    void Fill(const RGBA &);

    void FillRect(const Rect &, const RGBA &) const;

    void drawPixel(int x, int y, float brightness, uint32_t col);
    void drawPixelSafe(int x, int y, float brightness, const uint32_t col);

    void drawAALine(int x0, int y0, int x1, int y1, const RGBA &);

    void DrawLineAa(const Point &, const Point &, const RGBA &);

    void DrawLine(const Point &, const Point &, const RGBA &) const;

    void DrawPoint(const Point &, const RGBA &) const;

    void DrawRect(const Rect &, const RGBA &) const;

    void DrawBorder(const RGBA &, bool solid = true);

    virtual uint32_t GetMemoryUsage() const;

    std::string Info() const;

    Surface RenderScale(const Size &) const;

    Surface RenderReflect(int shape /* 0: none, 1 : vert, 2: horz, 3: both */) const;

    Surface RenderRotate(int parm /* 0: none, 1 : 90 CW, 2: 90 CCW, 3: 180 */) const;

    Surface RenderStencil(const RGBA &) const;

    Surface RenderContour(const RGBA &) const;

    Surface RenderGrayScale() const;

    Surface RenderSepia() const;

    Surface RenderChangeColor(const RGBA &, const RGBA &) const;

    Surface RenderSurface(const Rect &srcrt, const Size &) const;

    Surface RenderSurface(const Size &) const;

    virtual Surface GetSurface() const;

    virtual Surface GetSurface(const Rect &) const;

    static void SetDefaultPalette(SDL_Color *, int);

    static void SetDefaultDepth(uint32_t);

    static void SetDefaultColorKey(int, int, int);

    static void Swap(Surface &, Surface &);

    void SetAlphaMod(int);

    void Set(uint32_t sw, uint32_t sh, uint32_t bpp /* bpp: 8, 16, 24, 32 */, bool amask);

    void Lock() const;

    void SetPixel4(s32 x, s32 y, uint32_t color) const;

    void Unlock() const;

    SDL_Surface *surface;
protected:
    static void FreeSurface(Surface &);


    //void SetColorMod(const RGBA &);
    //void SetBlendMode(int);

    uint32_t MapRGB(const RGBA &) const;

    RGBA GetRGB(uint32_t pixel) const;

    void Set(const Surface &, bool refcopy);

    void Set(SDL_Surface *);

    void SetPalette();


    void SetPixel3(s32 x, s32 y, uint32_t color) const;

    void SetPixel2(s32 x, s32 y, uint32_t color) const;

    void SetPixel1(s32 x, s32 y, uint32_t color) const;

    void SetPixel(int x, int y, uint32_t) const;

    uint32_t GetPixel4(s32 x, s32 y) const;

    uint32_t GetPixel3(s32 x, s32 y) const;

    uint32_t GetPixel2(s32 x, s32 y) const;

    uint32_t GetPixel1(s32 x, s32 y) const;

    uint32_t GetPixel(int x, int y) const;
};
