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
#include <vector>
#include <ctime>

#include "system.h"
#include "gamedefs.h"
#include "text.h"
#include "agg.h"
#include "cursor.h"
#include "button.h"
#include "dialog.h"
#include "settings.h"
#include "world.h"
#include "zzlib.h"
#include "game.h"
#include "game_over.h"
#include "mus.h"
#include "audio.h"
#include "audio_mixer.h"
#include "audio_music.h"
#include "audio_cdrom.h"

#define HGS_ID    0xF1F3
#define HGS_MAX    10

struct hgs_t
{
    hgs_t() : localtime(0), days(0), rating(0)
    {};

    bool operator==(const hgs_t &) const;

    string player;
    string land;
    u32 localtime;
    u32 days;
    u32 rating;
};

StreamBase &operator<<(StreamBase &msg, const hgs_t &hgs)
{
    return msg << hgs.player << hgs.land << hgs.localtime << hgs.days << hgs.rating;
}

StreamBase &operator>>(StreamBase &msg, hgs_t &hgs)
{
    return msg >> hgs.player >> hgs.land >> hgs.localtime >> hgs.days >> hgs.rating;
}

bool hgs_t::operator==(const hgs_t &h) const
{
    return player == h.player && land == h.land && days == h.days;
}

bool RatingSort(const hgs_t &h1, const hgs_t &h2)
{
    return h1.rating > h2.rating;
}

class HGSData
{
public:
    HGSData() = default;

    bool Load(const string &);

    bool Save(const string &);

    void ScoreRegistry(const string &, const string &, u32, u32);

    void RedrawList(s32, s32);

private:
    vector<hgs_t> list;
};

bool HGSData::Load(const string &fn)
{
    ZStreamFile hdata;
    if (!hdata.read(fn)) return false;

    hdata.setbigendian(true);
    u16 hgs_id = 0;

    hdata >> hgs_id;

    if (hgs_id == HGS_ID)
    {
        hdata >> list;
        return !hdata.fail();
    }

    return false;
}

bool HGSData::Save(const string &fn)
{
    ZStreamFile hdata;
    hdata.setbigendian(true);
    hdata << static_cast<u16>(HGS_ID) << list;
    return !(hdata.fail() || !hdata.write(fn));

}

void HGSData::ScoreRegistry(const string &p, const string &m, u32 r, u32 s)
{
    hgs_t h;

    h.player = p;
    h.land = m;
    h.localtime = time(nullptr);
    h.days = r;
    h.rating = s;

    if (list.end() == find(list.begin(), list.end(), h))
    {
        list.push_back(h);
        sort(list.begin(), list.end(), RatingSort);
        if (list.size() > HGS_MAX) list.resize(HGS_MAX);
    }
}

void HGSData::RedrawList(s32 ox, s32 oy)
{
    const Settings &conf = Settings::Get();

    // image background
    const Sprite &back = AGG::GetICN(ICN::HSBKG, 0);
    back.Blit(ox, oy);

    const Sprite &head = AGG::GetICN(ICN::HISCORE, 6);
    head.Blit(ox + 50, oy + 31);

    sort(list.begin(), list.end(), RatingSort);

    vector<hgs_t>::const_iterator it1 = list.begin();
    vector<hgs_t>::const_iterator it2 = list.end();

    Text text;
    text.Set(Font::BIG);

    for (; it1 != it2 && (it1 - list.begin() < HGS_MAX); ++it1)
    {
        const hgs_t &hgs = *it1;

        text.Set(hgs.player);
        text.Blit(ox + 88, oy + 70);

        text.Set(hgs.land);
        text.Blit(ox + 260, oy + 70);

        text.Set(GetString(hgs.days));
        text.Blit(ox + 420, oy + 70);

        text.Set(GetString(hgs.rating));
        text.Blit(ox + 480, oy + 70);

        oy += 40;
    }
}

int Game::HighScores(bool fill)
{
    Cursor &cursor = Cursor::Get();
    Display &display = Display::Get();
    const Settings &conf = Settings::Get();

    cursor.Hide();
    if (fill) display.Fill(ColorBlack);

#ifdef WITH_DEBUG
    if(IS_DEVEL() && world.CountDay())
    {
    std::string msg = std::string("Devepoper mode, not save! \n \n Your result: ") + GetString(GetGameOverScores());
    Dialog::Message("High Scores", msg, Font::BIG, Dialog::OK);
    return MAINMENU;
    }
#endif

    HGSData hgs;

    ostringstream stream;
    stream << System::ConcatePath(conf.GetSaveDir(), "fheroes2.hgs");

    cursor.SetThemes(cursor.POINTER);
    Mixer::Pause();
    AGG::PlayMusic(MUS::MAINMENU);
    hgs.Load(stream.str());

    const Sprite &back = AGG::GetICN(ICN::HSBKG, 0);

    cursor.Hide();
    const Point top((display.w() - back.w()) / 2, (display.h() - back.h()) / 2);

    hgs.RedrawList(top.x, top.y);

    LocalEvent &le = LocalEvent::Get();

    Button buttonCampain(top.x + 9, top.y + 315, ICN::HISCORE, 0, 1);
    Button buttonExit(top.x + back.w() - 36, top.y + 315, ICN::HISCORE, 4,
                      5);

    buttonCampain.Draw();
    buttonExit.Draw();

    cursor.Show();
    display.Flip();

    const u32 rating = GetGameOverScores();
    const u32 days = world.CountDay();
    GameOver::Result &gameResult = GameOver::Result::Get();

    if (rating && (gameResult.GetResult() & GameOver::WINS))
    {
        string player(_("Unknown Hero"));
        Dialog::InputString(_("Your Name"), player);
        cursor.Hide();
        if (player.empty()) player = _("Unknown Hero");
        hgs.ScoreRegistry(player, Settings::Get().CurrentFileInfo().name, days, rating);
        hgs.Save(stream.str());
        hgs.RedrawList(top.x, top.y);
        buttonCampain.Draw();
        buttonExit.Draw();
        cursor.Show();
        display.Flip();
        gameResult.Reset();
    }

    // highscores loop
    while (le.HandleEvents())
    {
        // key code info
        if (Settings::Get().Debug() == 0x12 && le.KeyPress())
            Message("Key Press:", GetString(le.KeyValue()), Font::SMALL, Dialog::OK);
        le.MousePressLeft(buttonCampain) ? buttonCampain.PressDraw() : buttonCampain.ReleaseDraw();
        le.MousePressLeft(buttonExit) ? buttonExit.PressDraw() : buttonExit.ReleaseDraw();

        if (le.MouseClickLeft(buttonExit) || HotKeyCloseWindow) return MAINMENU;
    }

    return QUITGAME;
}
