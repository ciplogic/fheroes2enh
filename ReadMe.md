**FHeroes 2 Enhanced**

*What is it?*

Free Heroes 2 is an engine recreation of game Heroes 2 (of Might and Magic) with UI and coding enhancements.

*What do you mean by enhancements?*

Would you want to play Heroes 2 but you feel things missing like:
* Buying buildings with one click? 
* Upgrade units with one click if you have resources? 
* Last saved file is put on top of "Load" list?
* An installer that will also download the Heroes 2 Demo or extra maps to not look for them how to copy (only on Windows now)?
* Quick combat to avoid tedious battles?
* Performance improvements over Free Heroes 2?

If you want to see what is it about, and you have an Windows machine, why not try it?

The Releases page contains the latest version from here:
https://github.com/ciplogic/fheroes2enh/releases

<span style="color:red">**More details**</span>

Over time there will be mostly usability changes and improvements over modernizing the code. (i.e. C++ 11)

You can see a demo of how it runs and how it is enhanced here: (feel free to build it and make a more pleasing intro!)
https://youtu.be/AmPKviRAPB4

For now CMake is supported with Linux/Mac OS X only, but please help us to move the code to other platforms (MacOS, Windows and not only).

The editor will not be the target of this project.

**How to start?**

Identify your operating system and if is not Windows do the following: 

*Building on MacOS/Linux*


Install: 
- CMake

- C++ compiler

- SDL 1.2 Development package for SDL itself, SDL_Image, SDL_ttf, SDL_Mixer

- install an IDE of choice: like the excellent CLion from Jetbrains or 
a Makefile based IDE: Eclipse, NetBeans with C++ support and so on.

- download a copy of Heroes 2 Demo or make a copy of the original GoG excellent
Heroes 2 game.

- Clone this repository to a location. Make sure you work against master 
(or develop branch) to not be too late into development.

Copy the content of Heroes 2 Demo or Heroes 2 (especially the folders as DATA or MAPS)
into your source folder


Building the code:

- if you use CLion: chose CMakeLists.txt from the current folder, wait till the 
project is parsed by the IDE and click build

- if you use a Makefile based IDE which doesn't understand CMake: do run the command - 'cmake .' 
(run cmake in current directory) then you should be able to import the makefile in Eclipse CDT 
or any other tool

At the end of building, you should find *fheroes2* binary in the **cmake-build-*target*** folder. 
By default, the Release build uses **LTO** which can take some time on lower spec machines.


*Building on Windows*

Windows support will be always a secondary target from the point of the 
project but if you feel like supporting the Windows build, especially to integrate nicely 
with CMake, we will be able to make it work with you.

The long-story short is:

- install Visual Studio 2017 Community. It is free from Microsoft.

- install SDL development tools

- make a copy of the Git cloned folder into: C:\Cs-oss\FHeroes2Enh

- download your Heroes 2 installation (or Heroes 2 Demo installation) files into
C:\Cs-Oss\FHeroes2Enh

- Build it and it should run (at least on Debug configuration)

**How to contribute**

Excluding you have a clear vision of a feature, I recommend to make many pull requests
fixing one issue at a time. The smaller the better. 

If you don't know what to do, try to "clean up the code", especially consider 
into finding some code that is full with allocations/deletes for example, and try
rewrite it using std::shared_ptr or std::unique_ptr. Or try to use: std::string 
everyhwere it is used char*, or std::vector instead of plain arrays.

Even more important is to drop things that are not supported anymore:
- if you want to upgrade libraries like using SDL 2.0, today SDL 2.0 is 
incomplete (even broken), so, if you have time, making SDL 2.0 to run nicely 
and dropping SDL 1.2 would be really nice to have feature

- if you want to fix performance or animations or things like it: 
make sure you understand that section of code before rewriting it. Make 
sure that the code is more readable than it was before.

- if some code is hard to understand, rewrite it to be a bit more readable.
