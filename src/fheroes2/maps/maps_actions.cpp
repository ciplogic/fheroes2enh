/***************************************************************************
 *   Copyright (C) 2013 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#include "world.h"
#include "text.h"
#include "dialog.h"
#include "settings.h"
#include "game.h"
#include "maps_actions.h"

ByteVectorWriter& operator<<(ByteVectorWriter& sb, const ActionSimple& st)
{
    return sb << st.type << st.uid;
}

ByteVectorReader& operator>>(ByteVectorReader& sb, ActionSimple& st)
{
    return sb >> st.type >> st.uid;
}

ActionSimple::~ActionSimple() = default;

ByteVectorWriter& operator<<(ByteVectorWriter& sb, const ActionArtifact& st)
{
    return sb << static_cast<const ActionSimple &>(st) << st.artifact << st.message;
}

ByteVectorReader& operator>>(ByteVectorReader& sb, ActionArtifact& st)
{
    return sb >> static_cast<ActionSimple &>(st) >> st.artifact >> st.message;
}

ByteVectorWriter& operator<<(ByteVectorWriter& sb, const ActionAccess& st)
{
    return sb << static_cast<const ActionSimple &>(st) << st.allowPlayers << st.allowComputer
        << st.cancelAfterFirstVisit << st.message;
}

ByteVectorReader& operator>>(ByteVectorReader& sb, ActionAccess& st)
{
    return sb >> static_cast<ActionSimple &>(st) >> st.allowPlayers >> st.allowComputer >> st.cancelAfterFirstVisit
        >> st.message;
}

ByteVectorWriter& operator<<(ByteVectorWriter& sb, const ActionDefault& st)
{
    return sb << static_cast<const ActionSimple &>(st) << st.enabled << st.message;
}

ByteVectorReader& operator>>(ByteVectorReader& sb, ActionDefault& st)
{
    return sb >> static_cast<ActionSimple &>(st) >> st.enabled >> st.message;
}

ByteVectorWriter& operator<<(ByteVectorWriter& sb, const ActionMessage& st)
{
    return sb << static_cast<const ActionSimple &>(st) << st.message;
}

ByteVectorReader& operator>>(ByteVectorReader& sb, ActionMessage& st)
{
    return sb >> static_cast<ActionSimple &>(st) >> st.message;
}

ByteVectorWriter& operator<<(ByteVectorWriter& sb, const ListActions& listActions)
{
    sb << static_cast<uint32_t>(listActions.size());
    for (const auto& listAction : listActions)
    {
        sb << listAction->GetType();

        switch (listAction->GetType())
        {
        case ACTION_DEFAULT:
            {
                const auto* ptr = static_cast<const ActionDefault *>(listAction.get());
                if (ptr) sb << *ptr;
            }
            break;
        case ACTION_ACCESS:
            {
                const auto* ptr = static_cast<const ActionAccess *>(listAction.get());
                if (ptr) sb << *ptr;
            }
            break;
        case ACTION_MESSAGE:
            {
                const auto* ptr = static_cast<const ActionMessage *>(listAction.get());
                if (ptr) sb << *ptr;
            }
            break;
        case ACTION_RESOURCES:
            {
                const auto* ptr = static_cast<const ActionResources *>(listAction.get());
                if (ptr) sb << *ptr;
            }
            break;
        case ACTION_ARTIFACT:
            {
                const auto* ptr = static_cast<const ActionArtifact *>(listAction.get());
                if (ptr) sb << *ptr;
            }
            break;
        default:
            sb << *listAction;
            break;
        }
    }

    return sb;
}

ByteVectorReader& operator>>(ByteVectorReader& sb, MapActions& v)
{
    const uint32_t size = sb.get32();
    v.clear();
    for (uint32_t ii = 0; ii < size; ++ii)
    {
        s32 key;
        ListActions listActions;
        sb >> key;
        sb >> listActions;
        v.insert(std::make_pair(key, std::move(listActions)));
    }
    return sb;
}

ByteVectorReader& operator>>(ByteVectorReader& sb, ListActions& st)
{
    uint32_t size = 0;
    sb >> size;

    st.clear();

    for (uint32_t ii = 0; ii < size; ++ii)
    {
        int type;
        sb >> type;

        switch (type)
        {
        case ACTION_DEFAULT:
            {
                auto ptr = std::make_unique<ActionDefault>();
                sb >> *ptr;
                st.push_back(std::move(ptr));
            }
            break;
        case ACTION_ACCESS:
            {
                auto ptr = std::make_unique<ActionAccess>();
                sb >> *ptr;
                st.push_back(std::move(ptr));
            }
            break;
        case ACTION_MESSAGE:
            {
                auto ptr = std::make_unique<ActionMessage>();
                sb >> *ptr;
                st.push_back(std::move(ptr));
            }
            break;
        case ACTION_RESOURCES:
            {
                auto ptr = std::make_unique<ActionResources>();
                sb >> *ptr;
                st.push_back(std::move(ptr));
            }
            break;
        case ACTION_ARTIFACT:
            {
                auto ptr = std::make_unique<ActionArtifact>();
                sb >> *ptr;
                st.push_back(std::move(ptr));
            }
            break;

        default:
            {
                auto ptr = std::make_unique<ActionSimple>();
                sb >> *ptr;
                st.push_back(std::move(ptr));
            }
            break;
        }
    }

    return sb;
}

bool ActionAccess::Action(ActionAccess* act, s32 index, Heroes& hero)
{
    if (act)
    {
        if (act->cancelAfterFirstVisit &&
            hero.isVisited(world.GetTiles(index), Visit::GLOBAL))
            return false;

        if (!act->message.empty())
            Message("", act->message, Font::BIG, Dialog::OK);

        if (hero.isControlAI() && !act->allowComputer)
            return false;

        if (act->cancelAfterFirstVisit)
            hero.SetVisited(index, Visit::GLOBAL);

        if (hero.GetColor() & act->allowPlayers)
            return true;
    }

    return false;
}

bool ActionDefault::Action(ActionDefault* act, s32 index, Heroes& hero)
{
    if (act)
    {
        if (!act->message.empty())
            Message("", act->message, Font::BIG, Dialog::OK);
        return act->enabled;
    }

    return false;
}

bool ActionArtifact::Action(ActionArtifact* act, s32 index, Heroes& hero)
{
    if (act && act->artifact != Artifact::UNKNOWN)
    {
        if (!act->message.empty())
            Dialog::ArtifactInfo("", act->message, act->artifact);
        hero.PickupArtifact(act->artifact);
        act->artifact = Artifact::UNKNOWN;
        return true;
    }

    return false;
}

bool ActionResources::Action(ActionResources* act, s32 index, Heroes& hero)
{
    if (act && 0 < act->resources.GetValidItems())
    {
        Dialog::ResourceInfo("", act->message, act->resources);
        hero.GetKingdom().AddFundsResource(act->resources);
        act->resources.Reset();
        return true;
    }

    return false;
}

bool ActionMessage::Action(ActionMessage* act, s32 index, Heroes& hero)
{
    if (act)
    {
        if (!act->message.empty())
            Message("", act->message, Font::BIG, Dialog::OK);
        return true;
    }

    return false;
}
