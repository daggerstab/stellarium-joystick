Stellarium Joystick Plug-in
===========================

Plug-in providing joystick and gamepad support for Stellarium,
the free/open-source astronomy application (http://stellarium.org).

It is intended as a supplement to the existing forms of control (keyboard and
mouse), not as a replacement for them in all possible operations.

In very early stages of development. Current capabilities are very limited.
As it requires an additional dependency (the Simple DirectMedia Layer (SDL)
library), I've chosen to develop it as a stand-alone ("dynamic") plug-in.


Features
--------

At this stage of development:
 - the plug-in supports only one controlling device. If there are more than one
 connected to the system, it will pick the first indexed by SDL.
 - all controls are hard-coded. Customization is planned for the future.

Joystick controls:
 - the first two axes are assumed to be the X and Y axes and pan the view
 - the third axis (throttle? yaw?), if present, controls zoom
 - any hat switches, if present, pan the view
 - button 1 (trigger?) toggles the mount mode (between alt-azimuth and
 equatorial)
 - holding down button 2 allows finer movement when panning and zooming,
 similar to holding down SHIFT when using the keyboard.

Gamepad controlls:
 - the left analog stick (if present) pans the view
 - vertical axis of the right analog stick (if present) controls zoom
 - direction buttons pan the view
 - the button quartet on the right side:
    + the bottom button (X-cross or A) toggles the mount mode
    + holding down the right button (Circle or B) allows finer movement
    + the left button (Square or X) returns to the default zoom
    + the top bottom (Triangle or Y) returns to the current time, which is
    necessary, because...
 - the left shoulder button slows down time, the right one speeds it up


Configuration and customization
-------------------------------

At this stage of development, almost no configuration is possible.

If your gamepad is not recognized as a gamepad, it's possible to update or 
modify the database which identifies gamepads: the gamecontrollerdb.txt file.
It is read from the plug-in's data directory (see below).

You can download a copy of the SDL community-sourced database from its
repository at GitHub: https://github.com/gabomdq/SDL_GameControllerDB
The same page contains some instructions on how to get your gamepad's mappings
if you use Steam. You can also download the latest version of the database and
hope that your device is in it, directly from this link:
https://raw.githubusercontent.com/gabomdq/SDL_GameControllerDB/master/gamecontrollerdb.txt

The plug-in keeps its files in the modules/JoystickSupport sub-directory of
Stellarium's user directory. This means:
 + C:\Users\UserName\AppData\Roaming\Stellarium\modules\JoystickSupport (on
 Windows Vista, Windows 7 and presumably Windows 8)
 + C:\Documents and Settings\UserName\Application Data\Stellarium\modules\JoystickSupport
 (on Windows XP)
 + $HOME/Library/Preferences/Stellarium/modules/JoystickSupport (on
 Mac OS X)
 + ~/.stellarium/modules/JoystickSupport (on Linux)


Platforms
---------

The plug-in will aim to work on the same major platforms as Stellarium (Linux,
Mac OS X, Windows, and even BSD), but in practice:
 - Windows: it's very likely that the next release of Stellarium will not
 support dynamic plug-ins due to the way it's built (MSVC does not export all
 symbols like gcc does by default), unless a lot of work is done. At least
 one previous version of Stellarium (0.12.4) is supported though.
 - Mac OS X: I don't have anything running that, so any development will have to
 be done by an outsider.
 - Linux: the plug-in is currently developed on Ubuntu 13.10, but there's no
 binary package available. In the future, there can be a PPA on Launchpad,
 and/or I can submit it to Debian, from where it will percolate down to Ubuntu.


Development
-----------

This is a dynamic Stellarium plug-in (as opposed to the static, built-in default
plug-ins that are distributed with Stellarium). Since Stellarium lacks a proper
development package, in addition to the Stellarium binaries, to build
the plug-in you'll need Stellarium's source code for the same version.
On Windows, the plug-in will be able to link against libStelMain.dll only if
Stellarium had been compiled with MinGW/gcc. (Luckily, this covers 0.12.4 and
the most recent versions before that.) In all cases, it's very likely that
the resulting plug-in binary will be compatible only with the version of
Stellarium it was built against.

As this project is a work-in-progress, the code is mainly based on the current
Stellarium development version - around revision 6600 in the 'trunk' branch in
Stellarium's code repository. See https://code.launchpad.net/stellarium

Nevertheless, there is some support for previous versions - so far, it has been
successfully tested with 0.12.4 on Windows XP.

The plug-in  has the same build dependencies as Stellarium. Unfortunately,
it's necessary to use (almost) the same versions of the Qt libraries and
the same compiler as the ones used for the target version of Stellarium.
Fortunately, there seems to be some interoperability - patch versions are
supposed to be binary compatible (i.e. all 4.8.* versions are compatible,
all gcc 4.6.* versions use the same ABI, etc.).

Required:
 - for the development version, Qt 5.1 at this point (since you probably used
 the same libs for your Stellarium build, it's not a problem)
 - for Stellarium 0.12.4, on Windows, Qt 4.8.5 and MinGW with gcc 4.6
 (from the original MinGW project?) Installing the latter is somewhat difficult.
 - SDL2, version 2.0.1 or later; available from http://www.libsdl.org/ - you
 can use the pre-compiled development package on Windows

CMake is necessary to process the build script.
Source code documentation is based on Doxygen.

Before building, two paths need to be passed to CMake:
- STELLARIUM_SOURCE_DIR, the path to the src/ directory in Stellarium's
source package/version control working copy. 
- STELLARIUM_BINARY_DIR, the path to a directory that contains Stellarium's
executable, such as the build directory of a Stellarium build-from-source, or,
Stellarium's installation directory (if built on Windows with MinGW).

You can also pass a few optional parameters:
- STELLARIUM_VERSION is required when building the plug-in for a specific
version of Stellarium and should contain the version number. By default,
the development version is assumed. Example use: -DSTELLARIUM_VERSION=0.12.4 
- SDL2_DIR, the path to the main installation directory of SDL2
(if you don't have the environmental variable SDLDIR set)
- if you pass an empty value of CMAKE_INSTALL_PREFIX, the script will change it
to a suitable value, so running "make install" will install the plug-in in
Stellarium's user data directory. Alternatively, on Windows, setting it to
Stellarium's installation directory (e.g. C:\Program Files\Stellarium)
will install the plug-in there, allowing all users to use the same installation. 

If you game controller is not among the small number of controllers recognized
by SDL, you can augment the database by dropping a custom gamecontrollersdb.txt
in the plug-in's directory. A community-maintained controller database
can be found on Github: https://github.com/gabomdq/SDL_GameControllerDB
A copy will be included by default in the package some time in the future.


Installation
------------

The "install" target generated by CMake will hopefully copy the compiled
library file to the appropriate plug-in directory, so it can be found by
Stellarium and recognized as a plug-in. After that, it should be listed in
the "Plugins" tab of Stellarium's configuration window.

I will provide a more proper package in the future, including a Windows
installer, provided that the Windows build issues are ever resolved.
