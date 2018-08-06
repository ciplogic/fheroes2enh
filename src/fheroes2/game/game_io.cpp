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

#include <sstream>
#include <ctime>
#include "zzlib.h"
#include "text.h"
#include "settings.h"
#include "kingdom.h"
#include "dialog.h"
#include "world.h"
#include "game.h"
#include "game_over.h"
#include "game_static.h"
#include "game_io.h"
#include "ByteVectorReader.h"
#include "BinaryFileReader.h"
#include <chrono>
#include "FileUtils.h"
#include <iostream>

static u16 SAV2ID2 = 0xFF02;
static u16 SAV2ID3 = 0xFF03;

namespace Game
{
    struct HeaderSAV
    {
        enum
        {
            IS_COMPRESS = 0x8000, IS_LOYALTY = 0x4000
        };

        HeaderSAV() : status(0)
        {
        }

        HeaderSAV(const Maps::FileInfo &fi, bool loyalty) : status(0), info(fi)
        {
            time_t rawtime;
            time(&rawtime);
            info.localtime = rawtime;

            if (loyalty)
                status |= IS_LOYALTY;

#ifdef WITH_ZLIB
            status |= IS_COMPRESS;
#endif
        }

        u16 status;
        Maps::FileInfo info;
    };

    ByteVectorWriter&operator<<(ByteVectorWriter &msg, const HeaderSAV &hdr)
    {
        return msg << hdr.status << hdr.info;
    }
    
    ByteVectorReader &operator>>(ByteVectorReader &msg, HeaderSAV &hdr)
    {
        return msg >> hdr.status >> hdr.info;
    }
}
bool Game::Save(const string &fn)
{
    const bool autosave = System::GetBasename(fn) == "autosave.sav";
    const Settings &conf = Settings::Get();

    // ask overwrite?
    if (System::IsFile(fn) &&
        (!autosave && conf.ExtGameRewriteConfirm() || autosave && Settings::Get().ExtGameAutosaveConfirm()) &&
        Dialog::NO == Dialog::Message("", _("Are you sure you want to overwrite the save with this name?"), Font::BIG,
            Dialog::YES | Dialog::NO))
    {
        return false;
    }

    ByteVectorWriter bfs(250*1024);
    bfs.SetBigEndian(true);

    const u16 loadver = GetLoadVersion();
    if (!autosave) SetLastSavename(fn);


    bfs << static_cast<char>(SAV2ID3 >> 8) << static_cast<char>(SAV2ID3) <<
        Int2Str(loadver) << loadver << HeaderSAV(conf.CurrentFileInfo(), conf.PriceLoyaltyVersion());


    ByteVectorWriter bfz(226 * 1024);
    bfz.SetBigEndian(true);
    bfz << loadver << World::Get() << Settings::Get() <<
        GameOver::Result::Get() << GameStatic::Data::Get() << MonsterStaticData::Get() << SAV2ID3;
    bfs << bfz.data();
    const auto savedFileData = bfs.data();
    FileUtils::writeFileBytes(fn, savedFileData);
    return true;

}

//#define OLDMETHOD

bool Game::Load(const string &fn)
{
    Settings &conf = Settings::Get();
    // loading info
    ShowLoadMapsText();

    auto fileVector = FileUtils::readFileBytes(fn);
    if (fileVector.empty())
    {
        return false;
    }
    ByteVectorReader byteFs(fileVector);
    byteFs.setBigEndian(true);
    char major, minor;

    byteFs >> major >> minor;


    const u16 savid = static_cast<u16>(major) << 8 | static_cast<u16>(minor);

    // check version sav file
    if (savid != SAV2ID2 && savid != SAV2ID3)
    {
        return false;
    }

    string strver;
    u16 binver = 0;
    HeaderSAV header;

    // read raw info
    byteFs >> strver >> binver >> header;
    const size_t offset = byteFs.tell();


    if (header.status & HeaderSAV::IS_COMPRESS)
    {
        return false;
    }
    
    auto fileDataZ = FileUtils::readFileBytes(fn);
    ByteVectorReader fz(fileDataZ);

    fz.setBigEndian(true);
    if(fileDataZ.empty())
        return false;
    fileVector = FileUtils::readFileBytes(fn);
    if (fileVector.empty())
        return false;
    sp<ByteVectorReader> bfz = make_shared<ByteVectorReader>(fileVector);
    {
        bfz->setBigEndian(true);
        bfz->seek(offset);
        const int size = bfz->get32();
        fileVector = bfz->getRaw(size);
        bfz = make_shared<ByteVectorReader>(fileVector);
        bfz->setBigEndian(true);
    }

    if (header.status & HeaderSAV::IS_LOYALTY && !conf.PriceLoyaltyVersion())
    {
        Message("Warning", _("This file is saved in the \"Price Loyalty\" version.\nSome items may be unavailable."),
                Font::BIG, Dialog::OK);
    }

    fz >> binver;

    *bfz >> binver;

    // check version: false
    if (binver > CURRENT_FORMAT_VERSION || binver < LAST_FORMAT_VERSION)
    {
        ostringstream os;
        os << "usupported save format: " << binver << endl <<
           "game version: " << CURRENT_FORMAT_VERSION << endl <<
           "last version: " << LAST_FORMAT_VERSION;
        Message("Error", os.str(), Font::BIG, Dialog::OK);
        return false;
    }
    SetLoadVersion(binver);
    u16 end_check = 0;
    auto &world = World::Get();
    auto &settings = Settings::Get();
    auto &gameOverResult = GameOver::Result::Get();
    auto &gameStatic = GameStatic::Data::Get();
    auto &monsterData = MonsterStaticData::Get();

    *bfz >> world >> settings >>
         gameOverResult >> gameStatic
         >> monsterData
         >> end_check;
    World::Get().PostFixLoad();

    if ( end_check != SAV2ID2 && end_check != SAV2ID3)
    {
        return false;
    }

    SetLoadVersion(CURRENT_FORMAT_VERSION);

    SetLastSavename(fn);
    conf.SetGameType(conf.GameType() | TYPE_LOADFILE);

    return true;
}

bool Game::LoadSAV2FileInfo(const string &fn, Maps::FileInfo &finfo)
{
    auto fileBytes = FileUtils::readFileBytes(fn);
    if (fileBytes.empty())
        return false;
    ByteVectorReader fs(fileBytes);
    fs.setBigEndian(true);
    char major, minor;
    fs >> major >> minor;

    const u16 savid = static_cast<u16>(major) << 8 | static_cast<u16>(minor);

    // check version sav file
    if (savid != SAV2ID2 && savid != SAV2ID3)
        return false;

    string strver;
    u16 binver = 0;
    HeaderSAV header;

    // read raw info
    fs >> strver >> binver >> header;

    // hide: unsupported version
    if (binver > CURRENT_FORMAT_VERSION || binver < LAST_FORMAT_VERSION)
        return false;

    // check: compress game data
    if (header.status & HeaderSAV::IS_COMPRESS)
        return false;

    finfo = header.info;
    finfo.file = fn;

    return true;
}
