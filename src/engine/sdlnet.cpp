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

#ifdef WITH_NET

#include "sdlnet.h"

#include <sstream>
#include <iostream>


Network::Socket::Socket() : sd(nullptr), sdset(nullptr), status(0)
{
}

Network::Socket::Socket(const TCPsocket csd) : sd(nullptr), sdset(nullptr), status(0)
{
    Assign(csd);
}

Network::Socket::Socket(const Socket&) : sd(nullptr), sdset(nullptr), status(0)
{
}

Network::Socket& Network::Socket::operator=(const Socket&)
{
    return *this;
}

Network::Socket::~Socket()
{
    if (sd) Close();
}

void Network::Socket::Assign(const TCPsocket csd)
{
    if (sd) Close();

    if (csd)
    {
        sd = csd;
        sdset = SDLNet_AllocSocketSet(1);
        if (sdset) SDLNet_TCP_AddSocket(sdset, sd);
    }
}


uint32_t Network::Socket::Host() const
{
    IPaddress* remoteIP = sd ? SDLNet_TCP_GetPeerAddress(sd) : nullptr;
    if (remoteIP) return SDLNet_Read32(&remoteIP->host);
    H2ERROR(SDLNet_GetError());
    return 0;
}

u16 Network::Socket::Port() const
{
    IPaddress* remoteIP = sd ? SDLNet_TCP_GetPeerAddress(sd) : nullptr;
    if (remoteIP) return SDLNet_Read16(&remoteIP->port);
    H2ERROR(SDLNet_GetError());
    return 0;
}

bool Network::Socket::Ready() const
{
    return 0 < SDLNet_CheckSockets(sdset, 1) && 0 < SDLNet_SocketReady(sd);
}

#define ERROR_RECV 5 // any non zero ? // WEBOS
#define ERROR_SEND 6 // any non zero ? // WEBOS

bool Network::Socket::Recv(char* buf, int len)
{
    if (sd && buf && len)
    {
        int rcv = 0;

        while ((rcv = SDLNet_TCP_Recv(sd, buf, len)) > 0 && rcv < len)
        {
            buf += rcv;
            len -= rcv;
        }

        if (rcv != len)
            status |= ERROR_RECV;
    }

    return ! (status & ERROR_RECV);
}

bool Network::Socket::Send(const char* buf, int len)
{
    if (sd && len != SDLNet_TCP_Send(sd, (void*)buf, len))
        status |= ERROR_SEND;

    return ! (status & ERROR_SEND);
}

template <typename T>
T swap_endian(T u)
{
    static_assert (CHAR_BIT == 8, "CHAR_BIT != 8");

    union
    {
        T u;
        unsigned char u8[sizeof(T)];
    } source{}, dest{};

    source.u = u;

    for (size_t k = 0; k < sizeof(T); k++)
        dest.u8[k] = source.u8[sizeof(T) - k - 1];

    return dest.u;
}


bool Network::Socket::Recv32(uint32_t& v)
{
    if (Recv(reinterpret_cast<char*>(&v), sizeof v))
    {
        const auto swappedV = swap_endian(v);
        v = swappedV;
        return true;
    }
    return false;
}

bool Network::Socket::Recv16(u16& v)
{
    if (Recv(reinterpret_cast<char*>(&v), sizeof v))
    {
        const auto swappedV = swap_endian(v);
        v = swappedV;
        return true;
    }
    return false;
}

bool Network::Socket::Send32(const uint32_t& v0)
{
    uint32_t v = v0;
    const auto swappedV = swap_endian(v);
    v = swappedV;

    return Send(reinterpret_cast<char*>(&v), sizeof v);
}

bool Network::Socket::Send16(const u16& v0)
{
    u16 v = v0;
    const auto swappedV = swap_endian(v);
    v = swappedV;
    return Send(reinterpret_cast<char*>(&v), sizeof v);
}

bool Network::Socket::Open(IPaddress& ip)
{
    Assign(SDLNet_TCP_Open(&ip));

    if (! sd)
    H2ERROR(SDLNet_GetError());

    return sd;
}

bool Network::Socket::isValid() const
{
    return sd && 0 == status;
}

void Network::Socket::Close()
{
    if (sd)
    {
        if (sdset)
        {
            SDLNet_TCP_DelSocket(sdset, sd);
            SDLNet_FreeSocketSet(sdset);
            sdset = nullptr;
        }
        SDLNet_TCP_Close(sd);
        sd = nullptr;
    }
}

Network::Server::Server() = default;

TCPsocket Network::Server::Accept() const
{
    return SDLNet_TCP_Accept(sd);
}

bool Network::Init()
{
    if (SDLNet_Init() < 0)
    {
        H2ERROR(SDLNet_GetError());
        return false;
    }
    return true;
}

void Network::Quit()
{
    SDLNet_Quit();
}

bool Network::ResolveHost(IPaddress& ip, const char* host, u16 port)
{
    if (SDLNet_ResolveHost(&ip, host, port) < 0)
    {
        H2ERROR(SDLNet_GetError());
        return false;
    }
    return true;
}

const char* Network::GetError()
{
    return SDLNet_GetError();
}

#endif
