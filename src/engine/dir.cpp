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

#ifndef WIN32
#include <dirent.h>
#else 
#include <Windows.h>
#endif

#include "system.h"
#include "tools.h"

using namespace std;

wstring s2ws(const string& str)
{
	return wstring(str.begin(), str.end());
}

string ws2s(const wstring& wstr)
{
	using convert_typeX = codecvt_utf8<wchar_t>;
	wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(wstr);
}
void ListFiles::Append(const ListFiles &lst)
{
    insert(end(), lst.begin(), lst.end());
}

#ifdef WIN32
vector<string> GetFilesOfDir(const string &path)
{
	HANDLE hFind;
	WIN32_FIND_DATA FindFileData;
	auto wPath = s2ws(path + "\\*.*");
	vector<string> dirsInCurrent;
	if ((hFind = FindFirstFile(wPath.c_str(), &FindFileData)) != INVALID_HANDLE_VALUE) {
		do {
			wstring wFileName = FindFileData.cFileName;
			string fileName = ws2s(wFileName);
			string fullFileName = path + "\\" + fileName;
			if (System::IsFile(fullFileName))
			{
				dirsInCurrent.push_back(fullFileName);
			}
			

		} while (FindNextFile(hFind, &FindFileData));
		FindClose(hFind);
	}
	return dirsInCurrent;
}

void ListFiles::ReadDir(const string &path, const string &filter, bool sensitive)
{
	auto files = GetFilesOfDir(path);
	if (filter.empty())
	{
		for(auto fullname : files)
			push_back(fullname);
		return;
	}
	for (auto fullname : files)
	{
		auto insensitiveFile = StringLower(fullname);
		if (insensitiveFile.find(filter) != string::npos)
		{
			push_back(fullname);
		}
	}
}
#endif

#ifndef WIN32
void ListFiles::ReadDir(const string &path, const string &filter, bool sensitive)
{
	// read directory
	DIR *dp = opendir(path.c_str());

	if (dp)
	{
		struct dirent *ep;
		while (nullptr != (ep = readdir(dp)))
		{
			const std::string fullname = System::ConcatePath(path, ep->d_name);

			// if not regular file
			if (!System::IsFile(fullname)) continue;

			if (filter.empty())
            {
                push_back(fullname);
                continue;
            }

            std::string filename(ep->d_name);
            if (sensitive)
            {
                if (std::string::npos == filename.find(filter)) continue;
            }
            else
            {
                if (std::string::npos == StringLower(filename).find(StringLower(filter))) continue;
            }


            push_back(fullname);
		}
		closedir(dp);
	}
}

#endif


void ListDirs::Append(const vector<string> &dirs)
{
    insert(end(), dirs.begin(), dirs.end());
}
