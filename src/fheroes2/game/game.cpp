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

#include "system.h"
#include "gamedefs.h"
#include "settings.h"
#include "maps_tiles.h"
#include "world.h"
#include "agg.h"
#include "cursor.h"
#include "profit.h"
#include "buildinginfo.h"
#include "battle.h"
#include "difficulty.h"
#include "game_interface.h"
#include "game_static.h"
#include "ai.h"
#include "game.h"
#include "m82.h"
#include "mus.h"
#include "rand.h"
#include "audio.h"
#include "audio_mixer.h"
#include "audio_music.h"
#include "audio_cdrom.h"

namespace Game
{
    uint32_t GetMixerChannelFromObject(const Maps::Tiles &);

    void AnimateDelaysInitialize();

    void KeyboardGlobalFilter(int, int);

    void UpdateGlobalDefines(const string &);

    void LoadExternalResource(const Settings &);

    void HotKeysDefaults();

    void HotKeysLoad(const string &);

    bool disable_change_music = false;
    int current_music = MUS::UNKNOWN;
    uint32_t castle_animation_frame = 0;
    uint32_t maps_animation_frame = 0;
    string last_name;
    int save_version = CURRENT_FORMAT_VERSION;
    vector<int>
            reserved_vols(LOOPXX_COUNT, 0);
}

void Game::SetLoadVersion(int ver)
{
    save_version = ver;
}

int Game::GetLoadVersion()
{
    return save_version;
}

string Game::GetLastSavename()
{
    return last_name;
}

void Game::SetLastSavename(const string &name)
{
    last_name = name;
}

int Game::Testing(int t)
{
    return MAINMENU;
}

int Game::Credits()
{
    string str;
    str.reserve(200);

    str.append("version: ");
    str.append(Settings::GetVersion());
    str.append("\n \n");
    str.append("This program is distributed under the terms of the GPL v2.");
    str.append("\n");
    str.append("AI engine: ");
    str.append(AI::Type());
    str.append(", license: ");
    str.append(AI::License());
    str.append("\n \n");
    str.append("Site project:\n");
    str.append("https://github.com/ciplogic/fheroes2enh");
    str.append("\n \n");
    str.append("Authors:\n");
    str.append("Ciprian Khlud, maintainer\n");
    str.append("Andrey Afletdinov, (old)maintainer\n");
    str.append("email: ciprian.mustiata at gmail.com\n");

    Message("Free Heroes II Enhanced Engine", str, Font::SMALL, Dialog::OK);

    //VERBOSE("Credits: under construction.");

    return MAINMENU;
}

bool Game::ChangeMusicDisabled()
{
    return disable_change_music;
}

void Game::DisableChangeMusic(bool f)
{
    //disable_change_music = f;
}

void Game::Init()
{
    Settings &conf = Settings::Get();
    LocalEvent &le = LocalEvent::Get();

    // update all global defines
    if (conf.UseAltResource()) LoadExternalResource(conf);

    // default events
    le.SetStateDefaults();

    // set global events
    le.SetGlobalFilterMouseEvents(Cursor::Redraw);
    le.SetGlobalFilterKeysEvents(KeyboardGlobalFilter);
    le.SetGlobalFilter(true);

    le.SetTapMode(conf.ExtPocketTapMode());

    AnimateDelaysInitialize();

    HotKeysDefaults();

    const string hotkeys = Settings::GetLastFile("", "fheroes2.key");
    HotKeysLoad(hotkeys);
}

int Game::CurrentMusic()
{
    return current_music;
}

void Game::SetCurrentMusic(int mus)
{
    current_music = mus;
}

uint32_t &Game::MapsAnimationFrame()
{
    return maps_animation_frame;
}

uint32_t &Game::CastleAnimationFrame()
{
    return castle_animation_frame;
}

/* play all sound from focus area game */
void Game::EnvironmentSoundMixer()
{
    const Point abs_pt(Interface::GetFocusCenter());
    const Settings &conf = Settings::Get();

    if (!conf.Sound()) return;
    fill(reserved_vols.begin(), reserved_vols.end(), 0);

    // scan 4x4 square from focus
    for (s32 yy = abs_pt.y - 3; yy <= abs_pt.y + 3; ++yy)
    {
        for (s32 xx = abs_pt.x - 3; xx <= abs_pt.x + 3; ++xx)
        {
            if (!Maps::isValidAbsPoint(xx, yy))
                continue;
            const uint32_t channel = GetMixerChannelFromObject(world.GetTiles(xx, yy));
            if (channel >= reserved_vols.size()) continue;
            // calculation volume
            const int length = max(abs(xx - abs_pt.x), abs(yy - abs_pt.y));
            const int volume =
                    (2 < length ? 4 : (1 < length ? 8 : (0 < length ? 12 : 16))) * Mixer::MaxVolume() / 16;

            if (volume > reserved_vols[channel]) reserved_vols[channel] = volume;
        }
    }

    AGG::LoadLOOPXXSounds(reserved_vols);
}

uint32_t Game::GetMixerChannelFromObject(const Maps::Tiles &tile)
{
    // force: check stream
    if (tile.isStream()) return 13;

    return M82::GetIndexLOOP00XXFromObject(tile.GetObject(false));
}

uint32_t Game::GetRating()
{
    Settings &conf = Settings::Get();
    uint32_t rating = 50;

    switch (conf.MapsDifficulty())
    {
        case Difficulty::NORMAL:
            rating += 20;
            break;
        case Difficulty::HARD:
            rating += 40;
            break;
        case Difficulty::EXPERT:
        case Difficulty::IMPOSSIBLE:
            rating += 80;
            break;
        default:
            break;
    }

    switch (conf.GameDifficulty())
    {
        case Difficulty::NORMAL:
            rating += 30;
            break;
        case Difficulty::HARD:
            rating += 50;
            break;
        case Difficulty::EXPERT:
            rating += 70;
            break;
        case Difficulty::IMPOSSIBLE:
            rating += 90;
            break;
        default:
            break;
    }

    return rating;
}

uint32_t Game::GetGameOverScores()
{
    Settings &conf = Settings::Get();

    uint32_t k_size = 0;

    switch ((mapsize_t) conf.MapsSize().w)
    {
        case mapsize_t::SMALL:
            k_size = 140;
            break;
        case mapsize_t::MEDIUM:
            k_size = 100;
            break;
        case mapsize_t::LARGE:
            k_size = 80;
            break;
        case mapsize_t::XLARGE:
            k_size = 60;
            break;
        default:
            break;
    }

    uint32_t flag = 0;
    uint32_t nk = 0;
    uint32_t end_days = world.CountDay();

    for (uint32_t ii = 1; ii <= end_days; ++ii)
    {
        nk = ii * k_size / 100;

        if (0 == flag && nk > 60)
        {
            end_days = ii + (world.CountDay() - ii) / 2;
            flag = 1;
        } else if (1 == flag && nk > 120) end_days = ii + (world.CountDay() - ii) / 2;
        else if (nk > 180) break;
    }

    return GetRating() * (200 - nk) / 100;
}

void Game::ShowLoadMapsText()
{
    Display &display = Display::Get();
    const Rect pos(0, display.h() / 2, display.w(), display.h() / 2);
    TextBox text(_("Maps Loading..."), Font::BIG, pos.w);

    // blit test
    display.Fill(ColorBlack);
    text.Blit(pos, display);
    display.Flip();
}

uint32_t Game::GetLostTownDays()
{
    return GameStatic::GetGameOverLostDays();
}

uint32_t Game::GetViewDistance(uint32_t d)
{
    return GameStatic::GetOverViewDistance(d);
}

void Game::UpdateGlobalDefines(const string &spec)
{
}

uint32_t Game::GetWhirlpoolPercent()
{
    return GameStatic::GetLostOnWhirlpoolPercent();
}

void Game::LoadExternalResource(const Settings &conf)
{
    string spec;
    const string prefix_stats = System::ConcatePath("files", "stats");

    // globals.xml
    spec = Settings::GetLastFile(prefix_stats, "globals.xml");

    if (System::IsFile(spec))
        UpdateGlobalDefines(spec);

    // animations.xml
    spec = Settings::GetLastFile(prefix_stats, "animations.xml");

    if (System::IsFile(spec))
        Battle::UpdateMonsterSpriteAnimation(spec);

    // battle.xml
    spec = Settings::GetLastFile(prefix_stats, "battle.xml");

    if (System::IsFile(spec))
        Battle::UpdateMonsterAttributes(spec);

    // monsters.xml
    spec = Settings::GetLastFile(prefix_stats, "monsters.xml");

    if (System::IsFile(spec))
        Monster::UpdateStats(spec);

    // spells.xml
    spec = Settings::GetLastFile(prefix_stats, "spells.xml");

    if (System::IsFile(spec))
        Spell::UpdateStats(spec);

    // artifacts.xml
    spec = Settings::GetLastFile(prefix_stats, "artifacts.xml");

    if (System::IsFile(spec))
        Artifact::UpdateStats(spec);

    // buildings.xml
    spec = Settings::GetLastFile(prefix_stats, "buildings.xml");

    if (System::IsFile(spec))
        BuildingInfo::UpdateCosts(spec);

    // payments.xml
    spec = Settings::GetLastFile(prefix_stats, "payments.xml");

    if (System::IsFile(spec))
        PaymentConditions::UpdateCosts(spec);

    // profits.xml
    spec = Settings::GetLastFile(prefix_stats, "profits.xml");

    if (System::IsFile(spec))
        ProfitConditions::UpdateCosts(spec);

    // skills.xml
    spec = Settings::GetLastFile(prefix_stats, "skills.xml");

    if (System::IsFile(spec))
        Skill::UpdateStats(spec);
}

string Game::GetEncodeString(const string &str1)
{
    const Settings &conf = Settings::Get();

    // encode name
    if (conf.Unicode() && !conf.MapsCharset().empty())
        return EncodeString(str1, conf.MapsCharset().c_str());

    return str1;
}

int Game::GetKingdomColors()
{
    return Settings::Get().GetPlayers().GetColors();
}

int Game::GetActualKingdomColors()
{
    return Settings::Get().GetPlayers().GetActualColors();
}

#include <cmath>

string Game::CountScoute(uint32_t count, int scoute, bool shorts)
{
    double infelicity = 0;
    string res;

    switch (scoute)
    {
        case Skill::Level::BASIC:
            infelicity = count * 30 / 100.0;
            break;

        case Skill::Level::ADVANCED:
            infelicity = count * 15 / 100.0;
            break;

        case Skill::Level::EXPERT:
            res = shorts ? GetStringShort(count) : Int2Str(count);
            break;

        default:
            return Army::SizeString(count);
    }

    if (res.empty())
    {
        uint32_t min = Rand::Get(static_cast<uint32_t>(floor(count - infelicity + 0.5)),
                            static_cast<uint32_t>(floor(count + infelicity + 0.5)));
        uint32_t max = 0;

        if (min > count)
        {
            max = min;
            min = static_cast<uint32_t>(floor(count - infelicity + 0.5));
        } else
            max = static_cast<uint32_t>(floor(count + infelicity + 0.5));

        res = Int2Str(min);

        if (min != max)
        {
            res.append("-");
            res.append(Int2Str(max));
        }
    }

    return res;
}

void Game::PlayPickupSound()
{
    int wav = M82::UNKNOWN;

    switch (Rand::Get(1, 7))
    {
        case 1:
            wav = M82::PICKUP01;
            break;
        case 2:
            wav = M82::PICKUP02;
            break;
        case 3:
            wav = M82::PICKUP03;
            break;
        case 4:
            wav = M82::PICKUP04;
            break;
        case 5:
            wav = M82::PICKUP05;
            break;
        case 6:
            wav = M82::PICKUP06;
            break;
        case 7:
            wav = M82::PICKUP07;
            break;

        default:
            return;
    }

    AGG::PlaySound(wav);
}
