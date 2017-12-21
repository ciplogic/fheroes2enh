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

#include <iostream>

#include "engine.h"
#include "gamedefs.h"
#include "settings.h"
#include "agg.h"
#include "cursor.h"
#include "game.h"
//#include "test.h"
#include "images_pack.h"
#include "zzlib.h"

void LoadZLogo();

void SetVideoDriver(const string &);

void SetTimidityEnvPath(const Settings &);

void SetLangEnvPath(const Settings &);

void InitHomeDir();

void ReadConfigs();

int TestBlitSpeed();

int PrintHelp(const char *basename)
{
    COUT("Usage: " << basename << " [OPTIONS]");
#ifndef BUILD_RELEASE
    COUT("  -d\tdebug mode");
#endif
    COUT("  -h\tprint this help and exit");

    return EXIT_SUCCESS;
}

string GetCaption()
{
    return string("Free Heroes II, version: " + Settings::GetVersion());
}
#ifdef __APPLE__
int SDL_main(int argc, char **argv)
#elif WIN32
#include <windows.h>
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
#else
int main(int argc, char **argv)
#endif
{

    Settings &conf = Settings::Get();
    int test = 0;

    DEBUG(DBG_ALL, DBG_INFO, "Free Heroes II Enhanced, " + conf.GetVersion());
	vector<string> vArgv;


#ifndef WIN32
	for (int i = 0; i<argc; i++)
	{
		vArgv.emplace_back(argv[i]);
	}
#else   

	LPWSTR *szArglist;
	int nArgs;
	wstring commandLine = GetCommandLineW();

	szArglist = CommandLineToArgvW(commandLine.c_str(), &nArgs);
	for (int i = 0; i<nArgs; i++)
	{
		vArgv.emplace_back(ws2s(szArglist[i]));
	}
	LocalFree(szArglist);
#endif
    conf.SetProgramPath(vArgv[0].c_str());

    InitHomeDir();
    ReadConfigs();

    // getopt
    {
        int opt;
        while ((opt = System::GetCommandOptions(vArgv.size(), vArgv, "ht:d:")) != -1)
            switch (opt)
            {
#ifndef BUILD_RELEASE
                case 't':
        test = GetInt(System::GetOptionsArgument());
        break;

                case 'd':
                conf.SetDebug(System::GetOptionsArgument() ? GetInt(System::GetOptionsArgument()) : 0);
                break;
#endif
                case '?':
                case 'h':
                    return PrintHelp(vArgv[0].c_str());

                default:
                    break;
            }
    }

    if (!conf.SelectVideoDriver().empty()) SetVideoDriver(conf.SelectVideoDriver());

    // random init
    Rand::Init();
    if (conf.Music()) SetTimidityEnvPath(conf);

    u32 subsystem = INIT_VIDEO | INIT_TIMER;

    if (conf.Sound() || conf.Music())
        subsystem |= INIT_AUDIO;
#ifdef WITH_AUDIOCD
    if(conf.MusicCD())
        subsystem |= INIT_CDROM | INIT_AUDIO;
#endif
    if (SDL::Init(subsystem))
        {
            atexit(SDL::Quit);

            SetLangEnvPath(conf);

            if (Mixer::isValid())
            {
                Mixer::SetChannels(8);
                Mixer::Volume(-1, Mixer::MaxVolume() * conf.SoundVolume() / 10);
                Music::Volume(Mixer::MaxVolume() * conf.MusicVolume() / 10);
                if (conf.Music())
                {
                    Music::SetFadeIn(3000);
                }
            } else if (conf.Sound() || conf.Music())
            {
                conf.ResetSound();
                conf.ResetMusic();
            }

            if (0 == conf.VideoMode().w || 0 == conf.VideoMode().h)
                conf.SetAutoVideoMode();

            Display &display = Display::Get();
            display.SetVideoMode(conf.VideoMode().w, conf.VideoMode().h, conf.FullScreen());
            display.HideCursor();
            display.SetCaption(GetCaption().c_str());

            //Ensure the mouse position is updated to prevent bad initial values.
            LocalEvent::Get().GetMouseCursor();

            DEBUG(DBG_GAME, DBG_INFO, conf.String());
            DEBUG(DBG_GAME | DBG_ENGINE, DBG_INFO, display.GetInfo());

            // read data dir
            if (!AGG::Init())
                return EXIT_FAILURE;

            atexit(&AGG::Quit);

            conf.SetBlitSpeed(TestBlitSpeed());

            // init cursor
            Cursor::Get().SetThemes(Cursor::POINTER);

            // init game data
            Game::Init();

            // goto main menu
            int rs = (test ? Game::TESTING : Game::MAINMENU);

            while (rs != Game::QUITGAME)
            {
                switch (rs)
                {
                    case Game::MAINMENU:
                        rs = Game::MainMenu();
                        break;
                    case Game::NEWGAME:
                        rs = Game::NewGame();
                        break;
                    case Game::LOADGAME:
                        rs = Game::LoadGame();
                        break;
                    case Game::HIGHSCORES:
                        rs = Game::HighScores(true);
                        break;
                    case Game::CREDITS:
                        rs = Game::Credits();
                        break;
                    case Game::NEWSTANDARD:
                        rs = Game::NewStandard();
                        break;
                    case Game::NEWCAMPAIN:
                        rs = Game::NewCampain();
                        break;
                    case Game::NEWMULTI:
                        rs = Game::NewMulti();
                        break;
                    case Game::NEWHOTSEAT:
                        rs = Game::NewHotSeat();
                        break;
#ifdef NETWORK_ENABLE
                    case Game::NEWNETWORK:   
                		rs = Game::NewNetwork();		
                		break;
#endif
                    case Game::NEWBATTLEONLY:
                        rs = Game::NewBattleOnly();
                        break;
                    case Game::LOADSTANDARD:
                        rs = Game::LoadStandard();
                        break;
                    case Game::LOADCAMPAIN:
                        rs = Game::LoadCampain();
                        break;
                    case Game::LOADMULTI:
                        rs = Game::LoadMulti();
                        break;
                    case Game::SCENARIOINFO:
                        rs = Game::ScenarioInfo();
                        break;
                    case Game::SELECTSCENARIO:
                        rs = Game::SelectScenario();
                        break;
                    case Game::STARTGAME:
                        rs = Game::StartGame();
                        break;
                    case Game::TESTING:
                        rs = Game::Testing(test);
                        break;

                    default:
                        break;
                }
            }
        }
    return EXIT_SUCCESS;
}

int TestBlitSpeed()
{
    Display &display = Display::Get();
    Surface sf(display.GetSize(), true);
    Rect srcrt(0, 0, display.w() / 3, display.h());
    SDL::Time t;

    t.Start();
    sf.Fill(RGBA(0xFF, 0, 0));
    sf.Blit(srcrt, Point(0, 0), display);
    display.Flip();
    sf.Fill(RGBA(0, 0xFF, 0));
    sf.Blit(srcrt, Point(srcrt.w, 0), display);
    display.Flip();
    sf.Fill(RGBA(0, 0, 0xFF));
    sf.Blit(srcrt, Point(display.w() - srcrt.w, 0), display);
    display.Flip();
    sf.Fill(RGBA(0, 0, 0));
    sf.Blit(display);
    display.Flip();
    t.Stop();

    int res = t.Get();
    DEBUG(DBG_GAME | DBG_ENGINE, DBG_INFO, res);
    return res;
}

void LoadZLogo()
{
#ifdef BUILD_RELEASE
    string file = Settings::GetLastFile("image", "sdl_logo.png");
    // SDL logo
    if (Settings::Get().ExtGameShowSDL() && !file.empty())
    {
        Display &display = Display::Get();
        Surface sf;

        if (sf.Load(file))
        {
            Surface black(display.GetSize(), false);
            black.Fill(ColorBlack);

            // scale logo
            if (Settings::Get().QVGA())
                sf = Sprite::ScaleQVGASurface(sf);

            const Point offset((display.w() - sf.w()) / 2, (display.h() - sf.h()) / 2);

            display.Rise(sf, black, offset, 250, 500);
            display.Fade(sf, black, offset, 10, 500);
        }
    }
#endif
}

void ReadConfigs()
{
    Settings &conf = Settings::Get();
    ListFiles files = conf.GetListFiles("", "fheroes2.cfg");

	for (auto& file : files)
	{
		if (System::IsFile(file))
		{
			conf.Read(file);
		}
	}
}

void InitHomeDir()
{
    const string home = System::GetHomeDirectory("fheroes2");

    if (!home.empty())
    {
        const string home_maps = System::ConcatePath(home, "maps");
        const string home_files = System::ConcatePath(home, "files");
        const string home_files_save = System::ConcatePath(home_files, "save");

        if (!System::IsDirectory(home))
            System::MakeDirectory(home);

        if (System::IsDirectory(home, true) && !System::IsDirectory(home_maps))
            System::MakeDirectory(home_maps);

        if (System::IsDirectory(home, true) && !System::IsDirectory(home_files))
            System::MakeDirectory(home_files);

        if (System::IsDirectory(home_files, true) && !System::IsDirectory(home_files_save))
            System::MakeDirectory(home_files_save);
    }
}

void SetVideoDriver(const string &driver)
{
    System::SetEnvironment("SDL_VIDEODRIVER", driver.c_str());
}

void SetTimidityEnvPath(const Settings &conf)
{
    const string prefix_timidity = System::ConcatePath("files", "timidity");
    const string result = Settings::GetLastFile(prefix_timidity, "timidity.cfg");

    if (System::IsFile(result))
        System::SetEnvironment("TIMIDITY_PATH", System::GetDirname(result).c_str());
}

void SetLangEnvPath(const Settings &conf)
{
#ifdef WITH_TTF
    if(conf.Unicode())
    {
        System::SetLocale(LC_ALL, "");
        System::SetLocale(LC_NUMERIC, "C");

    std::string mofile = conf.ForceLang().empty() ?
        System::GetMessageLocale(1).append(".mo") :
        std::string(conf.ForceLang()).append(".mo");

    ListFiles translations = Settings::GetListFiles(System::ConcatePath("files", "lang"), mofile);

    if(translations.size())
    {
            if(Translation::bindDomain("fheroes2", translations.back().c_str()))
            Translation::setDomain("fheroes2");
    }
    else
        ERROR("translation not found: " + mofile);
    }
#endif
    Translation::setStripContext('|');
}
