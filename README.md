Stellarium Joystick Plug-in
===========================

Plug-in providing joystick and gamepad support for Stellarium,
the free/open-source astronomy application (http://stellarium.org).

In development.

Like Stellarium, the plug-in will aim to be multi-platform (Linux, Mac OS X,
Windows, and even BSD), but currently it supports only Linux, since it's
developed on that platform. It relies on the Simple DirectMedia Layer (SDL)
libraries for joystick handling, so the ultimate availability would be
determined by the intersection of platforms supported by Stellarium and SDL.

Development
-----------

This is a dynamic Stellarium plug-in (as opposed to the static, built-in default
plug-ins that are distributed with Stellarium). Therefore, you need
Stellarium's source code to build it.

As this is a work-in-progress, it's based on the current Stellarium development
version - around revision 6600 in the 'trunk' branch in Stellarium's Bazaar
repository on Launchpad (https://code.launchpad.net/stellarium).

The plug-in  has the same building dependencies as Stellarium (Qt 5.2 for the
development version, etc.) and SDL2 (version 2.0.1 or later; available from
http://www.libsdl.org/). CMake is necessary to process the build script.
Documentation will be based on Doxygen.

Before building, two paths need to be passed to CMake:
- STELLARIUM_SOURCE_DIR, the path to the src/ directory in Stellarium's
source package/version control working copy. 
- STELLARIUM_BINARY_DIR, the path to a directory that contains Stellarium's
executable, such as the build directory of a Stellarium build-from-source, or,
hopefully, Stellarium's installation directory (the latter is untested and
very probably, not yet supported)

