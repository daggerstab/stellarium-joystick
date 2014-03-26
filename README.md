Stellarium Joystick Plug-in
===========================

Plug-in providing joystick and gamepad support for Stellarium,
the free/open-source astronomy application (http://stellarium.org).

It is intended as a supplement to the existing forms of control (keyboard and
mouse), not as a replacement for them in all possible operations.

In very early stages of development. Current capabilities are very limited.
As it requires an additional dependency (the Simple DirectMedia Layer (SDL)
library), I've chosen to develop it as a stand-alone ("dynamic") plug-in.

Platforms
---------

The plug-in will aim to work on the same major platforms as Stellarium (Linux,
Mac OS X, Windows, and even BSD), but in practice:
 - Windows: it's very likely that the next release of Stellarium will not
 support dynamic plug-ins due to the way it's built (MSVC does not export all
 symbols like gcc does by default), unless a lot of work is done.
 - Mac OS X: I don't have anything running that, so any development will have to
 be done by an outsider.
 - Linux: the plug-in is currently developed on Ubuntu 13.10, but there's no
 binary package available. In the future, there can be a PPA on Launchpad,
 and/or I can submit it to Debian, from where it will percolate down to Ubuntu.

Development
-----------

This is a dynamic Stellarium plug-in (as opposed to the static, built-in default
plug-ins that are distributed with Stellarium). Since Stellarium lacks a proper
development package, you'll need Stellarium's source code to build the plug-in,
and it's likely that the resulting binary will be compatible only with your
Stellarium build. (Yes, that project is a mess.)

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

