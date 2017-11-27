/***************************************************************************
 *   Copyright (C) 2010 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of SDL++ Engine:                                                 *
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

#include <iterator>
#include <iostream>
#include <algorithm>

#include "tools.h"
#include "serialize.h"
#include "tinyconfig.h"
using namespace std;

bool SpaceCompare(char a, char b)
{
    return isspace(a) && isspace(b);
}

string ModifyKey(const string &str)
{
    string keyString = StringTrim(StringLower(str));

    // remove multiple space
    string::iterator it = unique(keyString.begin(), keyString.end(), SpaceCompare);
    keyString.resize(it - keyString.begin());

    // change space
    for(auto &c:keyString){
        if(isspace(c))
            c = ' ';
    }

    return keyString;
}

TinyConfig::TinyConfig(char sep, char com) : separator(sep), comment(com)
{
}

bool TinyConfig::Load(const string &cfile)
{
    StreamFile sf;
    if (!sf.open(cfile, "rb")) return false;

    auto rows = StringSplit(sf.toString(), "\n");

    for (auto &row : rows)
    {
        string str = StringTrim(row);
        if (str.empty() || str[0] == comment) continue;

        size_t pos = str.find(separator);
        if (string::npos != pos)
        {
            string left(str.substr(0, pos));
            string right(str.substr(pos + 1, str.length() - pos - 1));

            left = StringTrim(left);
            right = StringTrim(right);

            AddEntry(left, right, false);
        }
    }

    return true;
}

bool TinyConfig::Save(const string &cfile) const
{
    StreamFile sf;
    if (!sf.open(cfile, "wb")) return false;

    for (const auto& it : *this)
        sf << it.first << " " << separator << " " << it.second << '\n';

    return true;
}

void TinyConfig::Clear()
{
    clear();
}

void TinyConfig::AddEntry(const string &key, const string &val, bool uniq)
{
    iterator it = end();

    if (uniq &&
        (end() != (it = find(ModifyKey(key)))))
        it->second = val;
    else
        insert(std::pair<string, string>(ModifyKey(key), val));
}

void TinyConfig::AddEntry(const string &key, int val, bool uniq)
{
    iterator it = end();

    if (uniq &&
        (end() != (it = find(ModifyKey(key)))))
        it->second = GetString(val);
    else
        insert(std::pair<string, string>(ModifyKey(key), GetString(val)));
}

int TinyConfig::IntParams(const string &key) const
{
    auto it = find(ModifyKey(key));
    return it != end() ? GetInt(it->second) : 0;
}

string TinyConfig::StrParams(const string &key) const
{
    auto it = find(ModifyKey(key));
    return it != end() ? it->second : "";
}

vector<string> TinyConfig::ListStr(const string &key) const
{
    pair<const_iterator, const_iterator> ret = equal_range(ModifyKey(key));
    vector<string> res;

    for (auto it = ret.first; it != ret.second; ++it)
        res.push_back(it->second);

    return res;
}

vector<int> TinyConfig::ListInt(const string &key) const
{
    pair<const_iterator, const_iterator> ret = equal_range(ModifyKey(key));
    vector<int> res;

    for (auto it = ret.first; it != ret.second; ++it)
        res.push_back(GetInt(it->second));

    return res;
}

bool TinyConfig::Exists(const string &key) const
{
    return end() != find(ModifyKey(key));
}
