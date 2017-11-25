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

#ifndef H2SYSTEM_H
#define H2SYSTEM_H

#include <sstream>
#include <iostream>
#include <string>
#include <list>

#if defined(__SYMBIAN32__)
#define VERBOSE(x)
#elif defined(ANDROID)
#include <android/log.h>
namespace std
{
    static const char* android_endl = "\n";
}
#define endl android_endl
#define COUT(x) { std::ostringstream osss; osss << x; __android_log_print(ANDROID_LOG_INFO, "SDLHeroes2", "%s", osss.str().c_str()); }
#else
#define COUT(x) { std::cerr << x << std::endl; }
#endif

#define VERBOSE(x) { COUT(System::GetTime() << ": [VERBOSE]\t" << __FUNCTION__ << ":  " << x); }
#define ERROR(x) { COUT(System::GetTime() << ": [ERROR]\t" << __FUNCTION__ << ":  " << x); }

#include "dir.h"
#include <vector>
#include <functional>

namespace System
{
	struct ScopeExit
	{
		explicit ScopeExit(function<void()> action);
		~ScopeExit();
	private:
		function<void()> _action;

	};
    int SetEnvironment(const char *name, const char *value);

    const char *GetEnvironment(const char *name);

    int MakeDirectory(const string &);

    string ConcatePath(const string &, const string &);

    ListDirs GetDataDirectories(const string &);

    ListFiles GetListFiles(const string &, const string &, const string &);

    string GetHomeDirectory(const string &);

    string GetDirname(const string &);

    string GetBasename(const string &);

    string GetTime();

    void SetLocale(int, const char *);

    string GetMessageLocale(int /* 3: en_us.utf-8, 2: en_us, 1: en */);

    size_t GetMemoryUsage();

    int GetCommandOptions(int argc, vector<string> argv, const char *optstring);

    char *GetOptionsArgument();

    bool IsFile(const string &name, bool writable = false);

    bool IsDirectory(const string &name, bool writable = false);

    int Unlink(const string &);

    bool isEmbededDevice();

    bool isRunning();

    int CreateTrayIcon(bool);

    void PowerManagerOff(bool);

    int ShellCommand(const char *);

    int GetRenderFlags();
}

#endif
