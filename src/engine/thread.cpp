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

#include "system.h"
#include "thread.h"

using namespace SDL;

Thread::Thread() : thread(nullptr)
{
}

Thread::~Thread()
{
    Kill();
}

Thread::Thread(const Thread &) : thread(nullptr)
{
}

Thread &Thread::operator=(const Thread &)
{
    return *this;
}

void Thread::Create(int (*fn)(void *), void *param)
{
    thread = SDL_CreateThread(fn, param);
}

int Thread::Wait()
{
    int status = 0;
    if (thread) SDL_WaitThread(thread, &status);
    thread = nullptr;
    return status;
}

void Thread::Kill()
{
    if (thread) SDL_KillThread(thread);
    thread = nullptr;
}

bool Thread::IsRun() const
{
    return GetID();
}

u32 Thread::GetID() const
{
    return thread ? SDL_GetThreadID(thread) : 0;
}

Mutex::Mutex(bool init) : mutex(init ? SDL_CreateMutex() : nullptr)
{
}

Mutex::Mutex(const Mutex &) : mutex(nullptr)
{
}

Mutex::~Mutex()
{
    if (mutex) SDL_DestroyMutex(mutex);
}

Mutex &Mutex::operator=(const Mutex &)
{
    return *this;
}

void Mutex::Create()
{
    if (mutex) SDL_DestroyMutex(mutex);
    mutex = SDL_CreateMutex();
}

bool Mutex::Lock() const
{
    return mutex != nullptr && 0 == SDL_mutexP(mutex);
}

bool Mutex::Unlock() const
{
    return mutex != nullptr && 0 == SDL_mutexV(mutex);
}

Timer::Timer() : id(nullptr)
{
}

void Timer::Run(u32 interval, u32 (*fn)(u32, void *), void *param)
{
    if (id) Remove();

    id = SDL_AddTimer(interval, fn, param);
}

void Timer::Remove()
{
    if (id)
    {
        SDL_RemoveTimer(id);
        id = nullptr;
    }
}

bool Timer::IsValid() const
{
    return id;
}

Time::Time() = default;

void Time::Start()
{
    tick2 = tick1 = SDL_GetTicks();
}

void Time::Stop()
{
    tick2 = SDL_GetTicks();
}

u32 Time::Get() const
{
    return tick2 > tick1 ? tick2 - tick1 : 0;
}

void Time::Print(const char *header) const
{
    ERROR((header ? header : "time: ") << Get() << " ms");
}
