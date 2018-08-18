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
#include <sstream>
#include <functional>

#include "skill.h"
#include "settings.h"
#include "artifact.h"
#include "spell_storage.h"

SpellStorage::SpellStorage()
{
    reserve(67);
}

uint32_t SpellStorage::Size(int lvl) const
{
    switch (lvl)
    {
        case 1:
            return getSpellCountOfLevel(1);
        case 2:
            return getSpellCountOfLevel(2);
        case 3:
            return getSpellCountOfLevel(3);
        case 4:
            return getSpellCountOfLevel(4);
        case 5:
            return getSpellCountOfLevel(5);

        default:
            break;
    }

    return size();
}

long SpellStorage::getSpellCountOfLevel(int level) const
{
    return count_if(begin(), end(), [&](const Spell&s) {return s.isLevel(level); });
}

SpellStorage SpellStorage::GetSpells(int lvl) const
{
    SpellStorage result;
    result.reserve(20);
    for (auto it : *this)
        if (it.isLevel(lvl)) result.push_back(it);
    return result;
}

void SpellStorage::Append(const Spell &sp)
{
    if (sp != Spell::NONE &&
        end() == find(begin(), end(), sp))
        push_back(sp);
}

void SpellStorage::Append(const SpellStorage &st)
{
    insert(end(), st.begin(), st.end());
    sort(begin(), end());
    resize(unique(begin(), end()) - begin());
}

bool SpellStorage::isPresentSpell(const Spell &spell) const
{
    return end() != find(begin(), end(), spell);
}

string SpellStorage::String() const
{
    ostringstream os;

    for (auto it : *this)
        os << it.GetName() << ", ";

    return os.str();
}

void SpellStorage::Append(const BagArtifacts &bag)
{
    for (const auto it : bag)
        Append(it);
}

void SpellStorage::Append(const Artifact &art)
{
    switch (art())
    {
        case Artifact::SPELL_SCROLL:
            Append(Spell(art.GetSpell()));
            break;

        case Artifact::CRYSTAL_BALL:
            if (Settings::Get().ExtWorldArtifactCrystalBall())
            {
                Append(Spell(Spell::IDENTIFYHERO));
                Append(Spell(Spell::VISIONS));
            }
            break;

        case Artifact::BATTLE_GARB:
            Append(Spell(Spell::TOWNPORTAL));
            break;

        default:
            break;
    }
}
