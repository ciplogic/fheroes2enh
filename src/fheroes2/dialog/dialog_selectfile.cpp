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
#ifdef WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <algorithm>
#include <iterator>
#include <ctime>
#include <sstream>

#include "icn.h"

#include "system.h"
#include "agg.h"
#include "text.h"
#include "button.h"
#include "cursor.h"
#include "settings.h"
#include "interface_list.h"
#include "world.h"
#include "game.h"
#include "dialog.h"
#include "FileUtils.h"


string SelectFileListSimple(const string &, const string &, bool);

bool RedrawExtraInfo(const Point &, const string &, const string &, const Rect &);

class FileInfoListBox : public Interface::ListBox<Maps::FileInfo>
{
public:
    FileInfoListBox(const Point &pt, bool &edit) : ListBox<Maps::FileInfo>(pt), edit_mode(edit)
    {};

    void RedrawItem(const Maps::FileInfo &, s32, s32, bool);

    void RedrawBackground(const Point &);

    void ActionCurrentUp();

    void ActionCurrentDn();

    void ActionListDoubleClick(Maps::FileInfo &);

    void ActionListSingleClick(Maps::FileInfo &);

    void ActionListPressRight(Maps::FileInfo &)
    {};

    bool &edit_mode;
};

void FileInfoListBox::RedrawItem(const Maps::FileInfo &info, s32 dstx, s32 dsty, bool current)
{
    char short_date[20];
    time_t timeval = info.localtime;

    fill(short_date, ARRAY_COUNT_END(short_date), 0);
    strftime(short_date, ARRAY_COUNT(short_date) - 1, "%b %d, %H:%M", localtime(&timeval));
    string savname(System::GetBasename(info.file));

    if (!savname.empty())
    {
        Text text;
        const size_t dotpos = savname.size() - 4;
        if (StringLower(savname.substr(dotpos)) == ".sav") savname.erase(dotpos);

        text.Set(savname, (current ? Font::YELLOW_BIG : Font::BIG));
        text.Blit(dstx + 5, dsty, 155);

        text.Set(short_date, (current ? Font::YELLOW_BIG : Font::BIG));
        text.Blit(dstx + 265 - text.w(), dsty);
    }
}

void FileInfoListBox::RedrawBackground(const Point &dst)
{
    const Sprite &panel = AGG::GetICN(ICN::REQBKG, 0);

    panel.Blit(dst);
}

void FileInfoListBox::ActionCurrentUp()
{
    edit_mode = false;
}

void FileInfoListBox::ActionCurrentDn()
{
    edit_mode = false;
}

void FileInfoListBox::ActionListDoubleClick(Maps::FileInfo &)
{
    edit_mode = false;
}

void FileInfoListBox::ActionListSingleClick(Maps::FileInfo &)
{
    edit_mode = false;
}

string ResizeToShortName(const string &str)
{
    string res = System::GetBasename(str);
    size_t it = res.find('.');
    if (string::npos != it) res.resize(it);
    return res;
}

size_t GetInsertPosition(const string &name, s32 cx, s32 posx)
{
    if (name.empty())
        return 0;
    s32 tw = Text::width(name, Font::SMALL);
    if (cx <= posx) { return 0; }
     if (cx >= posx + tw)
        return name.size();
	float cw = tw / name.size();
	return static_cast<size_t>((cx - posx) / cw);
}


bool FileSortingByTime(const Maps::FileInfo &fi1, const Maps::FileInfo &fi2)
{
    long timeF1 = FileUtils::GetFileTime(fi1.file);
    long timeF2 = FileUtils::GetFileTime(fi2.file);
    return timeF1>timeF2;
}

MapsFileInfoList GetSortedMapsFileInfoList()
{
    ListFiles list1;
    list1.ReadDir(Settings::GetSaveDir(), ".sav", false);

    MapsFileInfoList list2(list1.size());
    int ii = 0;
    for (auto itd = list1.begin(); itd != list1.end(); ++itd, ++ii)
        if (!list2[ii].ReadSAV(*itd))--ii;
    if (static_cast<size_t>(ii) != list2.size()) list2.resize(ii);
    sort(list2.begin(), list2.end(), FileSortingByTime);

    return list2;
}

string Dialog::SelectFileSave()
{
    const Settings &conf = Settings::Get();
    const string &name = conf.CurrentFileInfo().name;

    string base = !name.empty() ? name : "newgame";
    base.resize(distance(base.begin(), find_if(base.begin(), base.end(), ::ispunct)));
    replace_if(base.begin(), base.end(), ::isspace, '_');
    ostringstream os;

    os << System::ConcatePath(Settings::GetSaveDir(), base) <<
       // add postfix:
       '_' << setw(3) << setfill('0') << world.CountDay() << ".sav";
    string lastfile = os.str();
    return SelectFileListSimple(_("File to Save:"), lastfile, true);
}

string Dialog::SelectFileLoad()
{
    const string lastfile = Game::GetLastSavename();
    return SelectFileListSimple(_("File to Load:"), (!lastfile.empty() ? lastfile : ""), false);
}

string SelectFileListSimple(const string &header, const string &lastfile, bool editor)
{
    Display &display = Display::Get();
    Cursor &cursor = Cursor::Get();
    LocalEvent &le = LocalEvent::Get();

    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    const Sprite &sprite = AGG::GetICN(ICN::REQBKG, 0);
    Size panel(sprite.w(), sprite.h());
    

    SpriteBack back(Rect((display.w() - panel.w) / 2, (display.h() - panel.h) / 2, panel.w, panel.h));

    const Rect &rt = back.GetArea();
    const Rect enter_field(rt.x + 42, rt.y + 286, 260, 16);

    bool edit_mode = false;

    MapsFileInfoList lists = GetSortedMapsFileInfoList();
    FileInfoListBox listbox(rt, edit_mode);

    listbox.RedrawBackground(rt);
    listbox.SetScrollButtonUp(ICN::REQUESTS, 5, 6, Point(rt.x + 327, rt.y + 55));
    listbox.SetScrollButtonDn(ICN::REQUESTS, 7, 8, Point(rt.x + 327, rt.y + 257));
    listbox.SetScrollSplitter(AGG::GetICN(ICN::ESCROLL, 3), Rect(rt.x + 328, rt.y + 73, 12, 180));
    listbox.SetAreaMaxItems(11);
    listbox.SetAreaItems(Rect(rt.x + 40, rt.y + 55, 265, 215));
    listbox.SetListContent(lists);

    string filename;
    size_t charInsertPos = 0;

    if (!lastfile.empty())
    {
        filename = ResizeToShortName(lastfile);
        charInsertPos = filename.size();

        auto it = lists.begin();
        for (; it != lists.end(); ++it) if ((*it).file == lastfile) break;

        if (it != lists.end())
            listbox.SetCurrent(distance(lists.begin(), it));
        else
            listbox.Unselect();
    }

    int btnTop = rt.y +  315;
    Button buttonOk(rt.x + 134, btnTop, ICN::REQUEST, 1, 2);
    Button buttonCancel(rt.x + 244, btnTop, ICN::REQUEST, 3, 4);

    if (!editor && lists.empty())
        buttonOk.SetDisable(true);

    if (filename.empty() && listbox.isSelected())
    {
        filename = ResizeToShortName(listbox.GetCurrent().file);
        charInsertPos = filename.size();
    }

    listbox.Redraw();
    RedrawExtraInfo(rt, header, filename, enter_field);

    std::function<void()> actionClear = [&]()
    {
        Rect sourceRect(buttonOk.x + 61, btnTop, 40, buttonOk.h+4);
        Point destPoint(rt.x + 31, btnTop);
        for(int step = 0; step<8; step++)
        {
            display.Blit(sourceRect, destPoint, display);
            destPoint.x += sourceRect.w;
        }

    };
    actionClear();

    buttonOk.Draw();
    buttonCancel.Draw();

    cursor.Show();
    display.Flip();

    string result;
    bool is_limit = false;

    while (le.HandleEvents() && result.empty())
    {
        le.MousePressLeft(buttonOk) && buttonOk.isEnable() ? buttonOk.PressDraw() : buttonOk.ReleaseDraw();
        le.MousePressLeft(buttonCancel) ? buttonCancel.PressDraw() : buttonCancel.ReleaseDraw();

        listbox.QueueEventProcessing();

        if ((buttonOk.isEnable() && le.MouseClickLeft(buttonOk)) || HotKeyPressEvent(Game::EVENT_DEFAULT_READY))
        {
            if (!filename.empty())
                result = System::ConcatePath(Settings::GetSaveDir(), filename + ".sav");
            else if (listbox.isSelected())
                result = listbox.GetCurrent().file;
        } else if (le.MouseClickLeft(buttonCancel) || HotKeyPressEvent(Game::EVENT_DEFAULT_EXIT))
        {
            break;
        } else if (le.MouseClickLeft(enter_field) && editor)
        {
            edit_mode = true;
            charInsertPos = GetInsertPosition(filename, le.GetMouseCursor().x, enter_field.x);
            buttonOk.SetDisable(filename.empty());
            cursor.Hide();
        } else if (edit_mode && le.KeyPress() &&
                   (!is_limit || KEY_BACKSPACE == le.KeyValue()))
        {
            charInsertPos = InsertKeySym(filename, charInsertPos, le.KeyValue(), le.KeyMod());
            buttonOk.SetDisable(filename.empty());
            cursor.Hide();
        }
        if ((le.KeyPress(KEY_DELETE)) && listbox.isSelected())
        {
            string msg(_("Are you sure you want to delete file:"));
            msg.append("\n \n");
            msg.append(System::GetBasename(listbox.GetCurrent().file));
            if (Dialog::YES == Dialog::Message(_("Warning!"), msg, Font::BIG, Dialog::YES | Dialog::NO))
            {
                System::Unlink(listbox.GetCurrent().file);
                listbox.RemoveSelected();
                if (lists.empty() || filename.empty()) buttonOk.SetDisable(true);
                listbox.SetListContent(lists);
            }
            cursor.Hide();
        }

        if (!cursor.isVisible())
        {
            listbox.Redraw();

            if (edit_mode && editor)
                is_limit = RedrawExtraInfo(rt, header, InsertString(filename, charInsertPos, "_"), enter_field);
            else if (listbox.isSelected())
            {
                filename = ResizeToShortName(listbox.GetCurrent().file);
                charInsertPos = filename.size();
                is_limit = RedrawExtraInfo(rt, header, filename, enter_field);
            } else
                is_limit = RedrawExtraInfo(rt, header, filename, enter_field);

            actionClear();
            buttonOk.Draw();
            buttonCancel.Draw();
            cursor.Show();
            display.Flip();
        }
    }

    cursor.Hide();
    back.Restore();

    return result;
}

bool RedrawExtraInfo(const Point &dst, const string &header, const string &filename, const Rect &field)
{
    Text text(header, Font::BIG);
    text.Blit(dst.x + 175 - text.w() / 2, dst.y + 30);

    if (!filename.empty())
    {
        text.Set(filename, Font::BIG);
        text.Blit(field.x, field.y + 1, field.w);
    }

    return text.w() + 10 > field.w;
}
