cmake_minimum_required(VERSION 3.5)
project(fheroes2)

set(CMAKE_CXX_STANDARD 14)

ADD_DEFINITIONS(-DBUILD_RELEASE -DWITH_AI=simple -DWITH_IMAGE -DWITH_MIXER -DWITH_TTF)

include_directories(
        src/engine
        src/engine/serializer
        src/fheroes2
        src/fheroes2/ai
        src/fheroes2/simple
        src/fheroes2/army
        src/fheroes2/agg
        src/fheroes2/battle
        src/fheroes2/castle
        src/fheroes2/dialog
        src/fheroes2/game
        src/fheroes2/gui
        src/fheroes2/heroes
        src/fheroes2/image
        src/fheroes2/kingdom
        src/fheroes2/pocketpc
        src/fheroes2/maps
        src/fheroes2/monster
        src/fheroes2/objects
        src/fheroes2/resource
        src/fheroes2/spell
        src/tools
        src/fheroes2/system)

set(SOURCE_FILES
        src/engine/audio.cpp
        src/engine/audio.h
        src/engine/audio_cdrom.cpp
        src/engine/audio_cdrom.h
        src/engine/audio_mixer.cpp
        src/engine/audio_mixer.h
        src/engine/audio_music.cpp
        src/engine/audio_music.h
        src/engine/dir.cpp
        src/engine/dir.h
        src/engine/display.cpp
        src/engine/display.h
        src/engine/engine.cpp
        src/engine/engine.h
        src/engine/error.cpp
        src/engine/error.h
        src/engine/font.cpp
        src/engine/font.h
        src/engine/IMG_savepng.cpp
        src/engine/IMG_savepng.h
        src/engine/localevent.cpp
        src/engine/localevent.h
        src/engine/rand.cpp
        src/engine/rand.h
        src/engine/rect.cpp
        src/engine/rect.h
        src/engine/sdlnet.cpp
        src/engine/sdlnet.h
        src/engine/sprites.cpp
        src/engine/sprites.h
        src/engine/surface.cpp
        src/engine/surface.h
        src/engine/system.cpp
        src/engine/system.h
        src/engine/thread.cpp
        src/engine/thread.h
        src/engine/tinyconfig.cpp
        src/engine/tools.cpp
        src/engine/tools.h
        src/engine/translations.cpp
        src/engine/translations.h
        src/engine/types.h
        src/engine/xmi2mid.cpp
        src/engine/serializer/ByteVectorReader.cpp
        src/engine/serializer/ByteVectorReader.h
        src/engine/serializer/BinaryFileReader.cpp
        src/engine/serializer/BinaryFileReader.h
        src/engine/serializer/ByteVectorWriter.cpp
        src/engine/serializer/ByteVectorWriter.h
        src/fheroes2/ai/simple/ai_battle.cpp
        src/fheroes2/ai/simple/ai_castle.cpp
        src/fheroes2/ai/simple/ai_heroes.cpp
        src/fheroes2/ai/simple/ai_kingdom.cpp
        src/fheroes2/ai/simple/ai_simple.cpp
        src/fheroes2/ai/simple/ai_simple.h
        src/fheroes2/ai/ai.h
        src/fheroes2/ai/ai_action.cpp
        src/fheroes2/army/army.cpp
        src/fheroes2/army/army.h
        src/fheroes2/army/army_bar.cpp
        src/fheroes2/army/army_bar.h
        src/fheroes2/army/army_troop.cpp
        src/fheroes2/army/army_troop.h
        src/fheroes2/battle/battle.h
        src/fheroes2/battle/battle_action.cpp
        src/fheroes2/battle/battle_arena.cpp
        src/fheroes2/battle/battle_arena.h
        src/fheroes2/battle/battle_army.cpp
        src/fheroes2/battle/battle_army.h
        src/fheroes2/battle/battle_board.cpp
        src/fheroes2/battle/battle_board.h
        src/fheroes2/battle/battle_bridge.cpp
        src/fheroes2/battle/battle_bridge.h
        src/fheroes2/battle/battle_catapult.cpp
        src/fheroes2/battle/battle_catapult.h
        src/fheroes2/battle/battle_cell.cpp
        src/fheroes2/battle/battle_cell.h
        src/fheroes2/battle/battle_command.cpp
        src/fheroes2/battle/battle_command.h
        src/fheroes2/battle/battle_dialogs.cpp
        src/fheroes2/battle/battle_force.h
        src/fheroes2/battle/battle_force.cpp
        src/fheroes2/battle/battle_grave.cpp
        src/fheroes2/battle/battle_grave.h
        src/fheroes2/battle/battle_interface.cpp
        src/fheroes2/battle/battle_interface.h
        src/fheroes2/battle/battle_main.cpp
        src/fheroes2/battle/battle_only.cpp
        src/fheroes2/battle/battle_only.h
        src/fheroes2/battle/battle_tower.cpp
        src/fheroes2/battle/battle_tower.h
        src/fheroes2/battle/battle_troop.cpp
        src/fheroes2/battle/battle_troop.h
        src/fheroes2/castle/buildinginfo.cpp
        src/fheroes2/castle/buildinginfo.h
        src/fheroes2/castle/captain.cpp
        src/fheroes2/castle/captain.h
        src/fheroes2/castle/castle.cpp
        src/fheroes2/castle/castle.h
        src/fheroes2/castle/castle_building.cpp
        src/fheroes2/castle/castle_dialog.cpp
        src/fheroes2/castle/castle_heroes.h
        src/fheroes2/castle/castle_mageguild.cpp
        src/fheroes2/castle/castle_tavern.cpp
        src/fheroes2/castle/castle_town.cpp
        src/fheroes2/castle/castle_well.cpp
        src/fheroes2/castle/mageguild.cpp
        src/fheroes2/castle/mageguild.h
        src/fheroes2/castle/marketplace.h
        src/fheroes2/dialog/dialog.h
        src/fheroes2/dialog/dialog_adventure.cpp
        src/fheroes2/dialog/dialog_arena.cpp
        src/fheroes2/dialog/dialog_armyinfo.cpp
        src/fheroes2/dialog/dialog_box.cpp
        src/fheroes2/dialog/dialog_buyboat.cpp
        src/fheroes2/dialog/dialog_chest.cpp
        src/fheroes2/dialog/dialog_file.cpp
        src/fheroes2/dialog/dialog_frameborder.cpp
        src/fheroes2/dialog/dialog_gameinfo.cpp
        src/fheroes2/dialog/dialog_giftresources.cpp
        src/fheroes2/dialog/dialog_guardian.cpp
        src/fheroes2/dialog/dialog_levelup.cpp
        src/fheroes2/dialog/dialog_marketplace.cpp
        src/fheroes2/dialog/dialog_message.cpp
        src/fheroes2/dialog/dialog_quickinfo.cpp
        src/fheroes2/dialog/dialog_recrut.cpp
        src/fheroes2/dialog/dialog_resourceinfo.cpp
        src/fheroes2/dialog/dialog_selectcount.cpp
        src/fheroes2/dialog/dialog_selectfile.cpp
        src/fheroes2/dialog/dialog_selectitems.cpp
        src/fheroes2/dialog/dialog_selectitems.h
        src/fheroes2/dialog/dialog_selectscenario.cpp
        src/fheroes2/dialog/dialog_selectscenario.h
        src/fheroes2/dialog/dialog_settings.cpp
        src/fheroes2/dialog/dialog_skillinfo.cpp
        src/fheroes2/dialog/dialog_spellinfo.cpp
        src/fheroes2/dialog/dialog_spriteinfo.cpp
        src/fheroes2/dialog/dialog_system.cpp
        src/fheroes2/dialog/dialog_thievesguild.cpp
        src/fheroes2/game/difficulty.cpp
        src/fheroes2/game/difficulty.h
        src/fheroes2/game/fheroes2.cpp
        src/fheroes2/game/game.cpp
        src/fheroes2/game/game.h
        src/fheroes2/game/game_delays.cpp
        src/fheroes2/game/game_highscores.cpp
        src/fheroes2/game/game_hotkeys.cpp
        src/fheroes2/game/game_interface.cpp
        src/fheroes2/game/game_interface.h
        src/fheroes2/game/game_io.cpp
        src/fheroes2/game/game_io.h
        src/fheroes2/game/game_loadgame.cpp
        src/fheroes2/game/game_mainmenu.cpp
        src/fheroes2/game/game_newgame.cpp
        src/fheroes2/game/game_over.cpp
        src/fheroes2/game/game_over.h
        src/fheroes2/game/game_scenarioinfo.cpp
        src/fheroes2/game/game_startgame.cpp
        src/fheroes2/game/game_static.cpp
        src/fheroes2/game/game_static.h
        src/fheroes2/gui/button.cpp
        src/fheroes2/gui/button.h
        src/fheroes2/gui/cursor.cpp
        src/fheroes2/gui/cursor.h
        src/fheroes2/gui/interface_border.cpp
        src/fheroes2/gui/interface_border.h
        src/fheroes2/gui/interface_buttons.cpp
        src/fheroes2/gui/interface_buttons.h
        src/fheroes2/gui/interface_cpanel.cpp
        src/fheroes2/gui/interface_cpanel.h
        src/fheroes2/gui/interface_events.cpp
        src/fheroes2/gui/interface_focus.cpp
        src/fheroes2/gui/interface_gamearea.cpp
        src/fheroes2/gui/interface_gamearea.h
        src/fheroes2/gui/interface_icons.cpp
        src/fheroes2/gui/interface_icons.h
        src/fheroes2/gui/interface_itemsbar.h
        src/fheroes2/gui/interface_list.h
        src/fheroes2/gui/interface_radar.cpp
        src/fheroes2/gui/interface_radar.h
        src/fheroes2/gui/interface_status.cpp
        src/fheroes2/gui/interface_status.h
        src/fheroes2/gui/interface_heroes_bar.cpp
        src/fheroes2/gui/interface_heroes_bar.h
        src/fheroes2/gui/splitter.cpp
        src/fheroes2/gui/splitter.h
        src/fheroes2/gui/statusbar.cpp
        src/fheroes2/gui/statusbar.h
        src/fheroes2/gui/text.cpp
        src/fheroes2/gui/text.h
        src/fheroes2/heroes/direction.cpp
        src/fheroes2/heroes/direction.h
        src/fheroes2/heroes/heroes.cpp
        src/fheroes2/heroes/heroes.h
        src/fheroes2/heroes/heroes_action.cpp
        src/fheroes2/heroes/heroes_base.cpp
        src/fheroes2/heroes/heroes_base.h
        src/fheroes2/heroes/heroes_dialog.cpp
        src/fheroes2/heroes/heroes_indicator.cpp
        src/fheroes2/heroes/heroes_indicator.h
        src/fheroes2/heroes/heroes_meeting.cpp
        src/fheroes2/heroes/heroes_move.cpp
        src/fheroes2/heroes/heroes_recruits.cpp
        src/fheroes2/heroes/heroes_recruits.h
        src/fheroes2/heroes/heroes_spell.cpp
        src/fheroes2/heroes/route.cpp
        src/fheroes2/heroes/route.h
        src/fheroes2/heroes/route_pathfind.cpp
        src/fheroes2/heroes/skill.cpp
        src/fheroes2/heroes/skill.h
        src/fheroes2/heroes/skill_static.h
        src/fheroes2/image/images_pack.h
        src/fheroes2/kingdom/color.cpp
        src/fheroes2/kingdom/color.h
        src/fheroes2/kingdom/kingdom.cpp
        src/fheroes2/kingdom/kingdom.h
        src/fheroes2/kingdom/kingdom_overview.cpp
        src/fheroes2/kingdom/luck.cpp
        src/fheroes2/kingdom/luck.h
        src/fheroes2/kingdom/morale.cpp
        src/fheroes2/kingdom/morale.h
        src/fheroes2/kingdom/payment.cpp
        src/fheroes2/kingdom/payment.h
        src/fheroes2/kingdom/profit.cpp
        src/fheroes2/kingdom/profit.h
        src/fheroes2/kingdom/puzzle.cpp
        src/fheroes2/kingdom/puzzle.h
        src/fheroes2/kingdom/race.cpp
        src/fheroes2/kingdom/race.h
        src/fheroes2/kingdom/speed.cpp
        src/fheroes2/kingdom/speed.h
        src/fheroes2/kingdom/week.cpp
        src/fheroes2/kingdom/week.h
        src/fheroes2/kingdom/world.cpp
        src/fheroes2/kingdom/world.h
        src/fheroes2/kingdom/world_loadmap.cpp
        src/fheroes2/maps/ground.cpp
        src/fheroes2/maps/ground.h
        src/fheroes2/maps/maps.cpp
        src/fheroes2/maps/maps.h
        src/fheroes2/maps/maps_actions.cpp
        src/fheroes2/maps/maps_actions.h
        src/fheroes2/maps/maps_fileinfo.cpp
        src/fheroes2/maps/maps_fileinfo.h
        src/fheroes2/maps/maps_objects.cpp
        src/fheroes2/maps/maps_objects.h
        src/fheroes2/maps/maps_tiles.cpp
        src/fheroes2/maps/maps_tiles.h
        src/fheroes2/maps/maps_tiles_quantity.cpp
        src/fheroes2/maps/mp2.cpp
        src/fheroes2/maps/mp2.h
        src/fheroes2/maps/pairs.h
        src/fheroes2/maps/position.cpp
        src/fheroes2/maps/position.h
        src/fheroes2/maps/visit.cpp
        src/fheroes2/maps/visit.h
        src/fheroes2/monster/monster.cpp
        src/fheroes2/monster/monster.h
        src/fheroes2/objects/mounts.cpp
        src/fheroes2/objects/mounts.h
        src/fheroes2/objects/objcrck.cpp
        src/fheroes2/objects/objcrck.h
        src/fheroes2/objects/objdirt.cpp
        src/fheroes2/objects/objdirt.h
        src/fheroes2/objects/objdsrt.cpp
        src/fheroes2/objects/objdsrt.h
        src/fheroes2/objects/objgras.cpp
        src/fheroes2/objects/objgras.h
        src/fheroes2/objects/objlava.cpp
        src/fheroes2/objects/objlava.h
        src/fheroes2/objects/objmult.cpp
        src/fheroes2/objects/objmult.h
        src/fheroes2/objects/objsnow.cpp
        src/fheroes2/objects/objsnow.h
        src/fheroes2/objects/objswmp.cpp
        src/fheroes2/objects/objswmp.h
        src/fheroes2/objects/objtown.cpp
        src/fheroes2/objects/objtown.h
        src/fheroes2/objects/objwatr.cpp
        src/fheroes2/objects/objwatr.h
        src/fheroes2/objects/objxloc.cpp
        src/fheroes2/objects/objxloc.h
        src/fheroes2/objects/trees.cpp
        src/fheroes2/objects/trees.h
        src/fheroes2/pocketpc/pocketpc.h
        src/fheroes2/pocketpc/pocketpc_dialog.cpp
        src/fheroes2/resource/artifact.cpp
        src/fheroes2/resource/artifact.h
        src/fheroes2/resource/artifact_ultimate.cpp
        src/fheroes2/resource/artifact_ultimate.h
        src/fheroes2/resource/maps_text.cpp
        src/fheroes2/resource/resource.cpp
        src/fheroes2/resource/resource.h
        src/fheroes2/spell/spell.cpp
        src/fheroes2/spell/spell.h
        src/fheroes2/spell/spell_book.cpp
        src/fheroes2/spell/spell_book.h
        src/fheroes2/spell/spell_storage.cpp
        src/fheroes2/spell/spell_storage.h
        src/fheroes2/system/bitmodes.h
        src/fheroes2/system/gamedefs.cpp
        src/fheroes2/system/gamedefs.h
        src/fheroes2/system/players.cpp
        src/fheroes2/system/players.h
        src/fheroes2/system/settings.cpp
        src/fheroes2/system/settings.h
        src/fheroes2/system/fh2_win32.cpp
        src/tools/palette_h2.h
        src/tools/palette_h2.cpp
        src/fheroes2/agg/ColorTable.h
        src/fheroes2/dialog/FileUtils.cpp
        src/fheroes2/dialog/FileUtils.h
        src/tools/palette_h2.cpp
        src/fheroes2/dialog/plus_sign_addon.cpp
        src/fheroes2/dialog/plus_sign_addon.h)

add_subdirectory(src)

add_executable(fheroes2 ${SOURCE_FILES})


INCLUDE(FindPkgConfig)

set(CMAKE_FIND_FRAMEWORK LAST)

#SDL
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(SDL_ttf REQUIRED)
find_package(PNG REQUIRED)


PKG_SEARCH_MODULE(SDL REQUIRED sdl)
PKG_SEARCH_MODULE(SDLIMAGE REQUIRED SDL_image>=1.2)
PKG_SEARCH_MODULE(SDLMIXER REQUIRED SDL_mixer>=1.2)
PKG_SEARCH_MODULE(SDLTTF REQUIRED SDL_ttf>=1.2)

INCLUDE_DIRECTORIES(${SDL_INCLUDE_DIRS} ${SDLIMAGE_INCLUDE_DIRS})


if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()


if(LINUX)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLMIXER_LIBRARY} ${SDLTTF_LIBRARY} -lSDL -lpng)
else()
    INCLUDE_DIRECTORIES(${SDL_INCLUDE_DIRS} /usr/local/include)
    link_directories(/usr/local/lib)
    link_libraries(libpng.a libSDL.a libSDLmain.a libSDL_image.a libSDL_mixer.a)

    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLMIXER_LIBRARY} ${SDLTTF_LIBRARY} -lpng -Wl,-framework,Cocoa )
endif()


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 ")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -ggdb -fno-omit-frame-pointer -O0 ")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -flto")
set(CMAKE_LD_FLAGS_RELEASE "${CMAKE_LD_FLAGS_RELEASE} -mtune=native -flto")
