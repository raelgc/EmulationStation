# EmulationStation Light

This is lightweight version of [Retropie EmulationStation](https://github.com/RetroPie/EmulationStation).

It has some of the modern Retropie features - like carousel themes, themes with variables (like [SNES mini](https://github.com/ruckage/es-theme-snes-mini)) - but using less resources than Retropie version (usually at least 1/3 of RAM usage, close to original ES), but no support for video, random, favorites or custom collections.

It's perfect for arcade cabinets, as it has a real kiosk mode (no config menus, Retropie config screen is hidden, exit only with F4 or shutdown) and when only 1 system is found, the gamelist view is automatically displayed.

Additionally it has and a handheld mode, where config menus are hidden but sound and shutdown menu are displayed.

The builtin Scraper was removed too (it's always broken!), so you can use [the Steven Selph's Scraper scraper](https://github.com/RetroPie/RetroPie-Setup/wiki/scraper#steven-selphs-scraper) on Retropie or [Universal XML Scraper](https://github.com/Universal-Rom-Tools/Universal-XML-Scraper) on Windows.

## Usage

You can use `--help` or `-h` to view a list of command-line options. Briefly outlined here:
```
--resolution [width] [height]	- try and force a particular resolution
--gamelist-only		- only display games defined in a gamelist.xml file.
--ignore-gamelist	- do not parse any gamelist.xml files.
--draw-framerate	- draw the framerate.
--debug			- show the console window on Windows, do slightly more logging
--windowed	- run ES in a window, works best in conjunction with --resolution [w] [h].
--vsync [1/on or 0/off]	- turn vsync on or off (default is on).
--no-splash		- don't show the splash screen.
--force-handheld		- hide all configurations
--force-kiosk		- hide all configurations, don't display any menus, including exit
```

On Kiosk mode, exit will work only with F4 or system shutdown.

## Install

### Retropie/Raspbian

First exit EmulationStation.

Second, install required packages:

```bash
sudo apt install libc6, libsdl2-2.0-0, libboost-system1.65.1, libboost-filesystem1.65.1, libfreeimage3, libfreetype6, libcurl4, libasound2, libpugixml1v5
```

Then download the proper binary (this is the one for Retropie based on Raspbian Stretch, check others in [Release page](https://github.com/raelgc/EmulationStation/releases/)):

```bash
wget https://github.com/raelgc/EmulationStation/releases/download/2.7.4-light/emulationstation-light-pi3-raspbian-stretch
```

Make a backup of `retropie` EmulationStation:

```bash
sudo cp /opt/retropie/supplementary/emulationstation/emulationstation /opt/retropie/supplementary/emulationstation/emulationstation.retropie.bak
```

Replace with the new one:

```bash
sudo cp emulationstation-light-pi3-raspbian-stretch /opt/retropie/supplementary/emulationstation/emulationstation
```

Done! 

### Ubuntu 18.04

Repeat above instructions, but using the [Ubuntu 18.04 binary](https://github.com/raelgc/EmulationStation/releases/download/2.7.4-light/emulationstation-light-amd64-ubuntu-18.04).

## Building

To build from source code, after clone this repo, install required build packages:

```bash
sudo apt-get install cmake build-essential libsdl2-dev libboost-system-dev libboost-filesystem-dev libboost-date-time-dev libpugixml-dev libfreeimage-dev libfreetype6-dev libeigen3-dev libcurl4-openssl-dev libasound2-dev libgl1-mesa-dev
```

If you want fallback fonts for Chinese/Japanese/Korean during runtime, install `fonts-droid` (sometimes package is named `fonts-droid-fallback`).

Generate and build the Makefile with CMake:
```bash
cd emulationstation
cmake .
make
```

### On Windows

Install all required libraries:

[Boost](http://www.boost.org/users/download/) (you'll need to compile yourself or get the pre-compiled binaries)

[Eigen3](http://eigen.tuxfamily.org/index.php?title=Main_Page) (header-only library)

[FreeImage](http://downloads.sourceforge.net/freeimage/FreeImage3154Win32.zip)

[FreeType2](http://download.savannah.gnu.org/releases/freetype/freetype-2.4.9.tar.bz2) (you'll need to compile)

[SDL2](http://www.libsdl.org/release/SDL2-devel-2.0.3-VC.zip)

[pugixml](https://pugixml.org/)

[cURL](http://curl.haxx.se/download.html) (you'll need to compile or get the pre-compiled DLL version)

Remember to copy necessary .DLLs into the same folder as the executable: probably FreeImage.dll, freetype6.dll, SDL2.dll, libcurl.dll, and zlib1.dll. Exact list depends on if you built your libraries in "static" mode or not. For pugixml, just include the sources.

[CMake](http://www.cmake.org/cmake/resources/software.html) (this is used for generating the Visual Studio project)

If you don't know how to use CMake, here are some hints: run `cmake-gui` and point it at your EmulationStation folder.  Point the "build" directory somewhere - I use EmulationStation/build.  Click configure, choose "Visual Studio [year] Project", fill in red fields as they appear and keep clicking Configure (you may need to check "Advanced"), then click Generate.

# Credits

As always, all credits to [Retropie](https://retropie.org.uk/) project and [Aloshi](http://www.aloshi.com), the original author of [EmulationStation](http://www.emulationstation.org).