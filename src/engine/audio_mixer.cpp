/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#include "engine.h"

#include "system.h"
#include "tools.h"

#include "audio.h"
#include "audio_mixer.h"
#include "audio_music.h"
#include <iostream>

namespace Mixer
{
    void Init();

    void Quit();

    bool valid = false;
}

bool Mixer::isValid()
{
    return valid;
}

#include "SDL_mixer.h"

void FreeChannel(int channel)
{
    Mixer::chunk_t* sample = Mix_GetChunk(channel);
    if (sample) Mix_FreeChunk(sample);
}

void Mixer::Init()
{
    if (!SDL::SubSystem(SDL_INIT_AUDIO))
    {
        H2ERROR("audio subsystem not initialize");
        valid = false;
        return;
    }

    Audio::Spec& hardware = Audio::GetHardwareSpec();
    hardware.freq = 22050;
    hardware.format = AUDIO_S16;
    hardware.channels = 2;
    hardware.samples = 2048;

    if (0 != Mix_OpenAudio(hardware.freq, hardware.format, hardware.channels, hardware.samples))
    {
        H2ERROR(SDL_GetError());
        valid = false;
        return;
    }

    int channels = 0;
    Mix_QuerySpec(&hardware.freq, &hardware.format, &channels);
    hardware.channels = channels;
    valid = true;
}

void Mixer::Quit()
{
    if (!SDL::SubSystem(SDL_INIT_AUDIO) || !valid)
        return;
    Music::Reset();
    Reset();
    valid = false;
    Mix_CloseAudio();
}

void Mixer::SetChannels(u8 num)
{
    Mix_AllocateChannels(num);
    Mix_ReserveChannels(1);
}

void Mixer::FreeChunk(chunk_t* sample)
{
    if (sample) Mix_FreeChunk(sample);
}


Mixer::chunk_t* Mixer::LoadWAV(const char* file)
{
    Mix_Chunk* sample = Mix_LoadWAV(file);
    if (!sample)
    H2ERROR(SDL_GetError());
    return sample;
}

Mixer::chunk_t* Mixer::LoadWAV(const u8* ptr, uint32_t size)
{
    Mix_Chunk* sample = Mix_LoadWAV_RW(SDL_RWFromConstMem(ptr, size), 1);
    if (!sample)
    H2ERROR(SDL_GetError());
    return sample;
}

int Mixer::Play(chunk_t* sample, int channel, bool loop)
{
    const int res = Mix_PlayChannel(channel, sample, loop ? -1 : 0);
    if (res == -1)
    H2ERROR(SDL_GetError());
    return res;
}

int Mixer::Play(const char* file, int channel, bool loop)
{
    if (!valid)
        return -1;

    chunk_t* sample = LoadWAV(file);
    if (!sample)
        return -1;
    Mix_ChannelFinished(FreeChannel);
    return Play(sample, channel, loop);
}

int Mixer::Play(const u8* ptr, uint32_t size, int channel, bool loop)
{
    if (!valid || !ptr)
    {
        return -1;
    }
    chunk_t* sample = LoadWAV(ptr, size);
    if (!sample)
        return -1;
    Mix_ChannelFinished(FreeChannel);
    return Play(sample, channel, loop);
}

u16 Mixer::MaxVolume()
{
    return MIX_MAX_VOLUME;
}

u16 Mixer::Volume(int channel, s16 vol)
{
    if (!valid) return 0;
    return Mix_Volume(channel, vol > MIX_MAX_VOLUME ? MIX_MAX_VOLUME : vol);
}

void Mixer::Pause(int channel)
{
    Mix_Pause(channel);
}

void Mixer::Resume(int channel)
{
    Mix_Resume(channel);
}

void Mixer::Stop(int channel)
{
    Mix_HaltChannel(channel);
}

void Mixer::Reset()
{
    Music::Reset();
#ifdef WITH_AUDIOCD
    if(Cdrom::IsValid()) Cdrom::Pause();
#endif
    Mix_HaltChannel(-1);
}

u8 Mixer::isPlaying(int channel)
{
    return Mix_Playing(channel);
}

u8 Mixer::isPaused(int channel)
{
    return Mix_Paused(channel);
}

void Mixer::Reduce()
{
}

void Mixer::Enhance()
{
}
