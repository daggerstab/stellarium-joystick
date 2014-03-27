/*
Stellarium Joystick Plug-in
Copyright (C) 2014  Bogdan Marinov <daggerstab@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "JoystickSupport.hpp"
#include "StelFileMgr.hpp"

#include <QDebug>

#include "StelApp.hpp"
#include "StelCore.hpp"
#include "StelMovementMgr.hpp"

StelModule*
JoystickPluginInterface::getStelModule() const
{
	return new JoystickSupport();
}

StelPluginInfo
JoystickPluginInterface::getPluginInfo() const
{
	StelPluginInfo info;
	info.id = "JoystickSupport";
	info.displayedName = "Joystick Support";
	info.authors = "Bogdan Marinov";
	info.contact = "daggerstab@gmail.com";
	info.description = "Basic support for joysticks and gamepads (joypads, video game controllers).";
	info.version = "0.0.2"; // TODO: Handle version numbers in CMake?
	return info;
}



JoystickSupport::JoystickSupport() : initialized(false), joystick(NULL)
{
	setObjectName("JoystickSupport");
}

JoystickSupport::~JoystickSupport()
{
	// TODO: Destructor
}


void
JoystickSupport::init()
{
	SDL_SetMainReady(); // Because SDL's custom main() is not used
	 // Implies also SDL_INIT_JOYSTICK
	if (SDL_Init(SDL_INIT_GAMECONTROLLER) != 0)
	{
		qWarning() << "JoystickSupport: SDL failed to initialize:"
		           << SDL_GetError();
		initialized = false;
		return;
	}
	initialized = true;

	// For debugging:
	devicesDescribed = false;

	// TODO: Disable joystick/gamepad event handling?
	SDL_JoystickEventState(SDL_IGNORE);
	SDL_GameControllerEventState(SDL_IGNORE);

	QString dbPath = StelFileMgr::findFile("modules/JoystickSupport/gamecontrollerdb.txt",
	                                       StelFileMgr::File);
	if (!dbPath.isEmpty())
	{
		qDebug() << "JoystickSupport: loading game controller database:"
		         << dbPath;
		SDL_RWops* dbFileStream = SDL_RWFromFile(dbPath.toUtf8().data(), "rt");
		if (dbFileStream)
		{
			// Second param indicates that the stream should be closed on finish
			int count = SDL_GameControllerAddMappingsFromRW(dbFileStream, 1);
			if (count > 0)
				qDebug() << "JoystickSupport:" << count
				         << "device descriptions loaded.";
		}
		else
			qDebug() << "JoystickSupport: SDL error:" << SDL_GetError();
	}
}

void
JoystickSupport::deinit()
{
	if (joystick)
	{
		SDL_JoystickClose(joystick);
		joystick = NULL;
	}
	if (initialized)
		SDL_Quit();
}

void
JoystickSupport::update(double deltaTime)
{
	if (!initialized)
		return;

	int deviceCount = SDL_NumJoysticks();
	if (deviceCount < 0)
	{
		qWarning() << "JoystickSupport: error finding number of devices:"
		           << SDL_GetError();
		return;
	}
	if (deviceCount == 0)
	{
		if (joystick)
		{
			SDL_JoystickClose(joystick);
			joystick = NULL;
		}
		return;
	}
	// TODO: Emit signal if there is a change in connected number?

	if (!devicesDescribed)
	{
		devicesDescribed = true;
		printDeviceDescriptions();
	}

	// TODO: For now assuming that we'll only use joystick 0
	int index = 0;
	if (joystick == NULL)
	{
		joystick = SDL_JoystickOpen(index);
		if (joystick == NULL)
			qWarning() << "JoystickSupport: unable to open device" << index
			           << SDL_GetError();
	}

	if (SDL_JoystickGetAttached(joystick) == SDL_FALSE)
	{
		SDL_JoystickClose(joystick);
		joystick = NULL;
	}

	if (joystick)
	{
		SDL_JoystickUpdate();
		//SDL_GameControllerUpdate();

		StelCore* core = StelApp::getInstance().getCore();
		handleJoystickButtons(core);
		handleJoystickHats(core);
	}
}

bool
JoystickSupport::configureGui(bool show)
{
	// TODO: Think of a way to make GUI more independent for all plugins.
	Q_UNUSED(show);
	return false; // For now there's no configuration window
}

void
JoystickSupport::printDeviceDescriptions()
{
	int deviceCount = SDL_NumJoysticks();
	qDebug() << "JoystickSupport: Number of connected devices:" << deviceCount;

	for (int i = 0; i < deviceCount; i++) // Thread safety?
	{
		bool isGamepad = SDL_IsGameController(i);
		qDebug() << "Device" << i << ':'
		         << QString(SDL_JoystickNameForIndex(i))
		         << (isGamepad ? "is a game controller" : "is a joystick");

		SDL_GameController* gamepad = NULL;
		SDL_Joystick* joystick = NULL;
		if (isGamepad)
		{
			gamepad = SDL_GameControllerOpen(i);
			if (gamepad == NULL)
			{
				qDebug() << "JoystickSupport: unable to open device" << i;
				continue;
			}

			// TODO: Load and display gamepad bindings?

			joystick = SDL_GameControllerGetJoystick(gamepad);
		}
		else
			joystick = SDL_JoystickOpen(i);

		if (joystick == NULL)
		{
			qDebug() << "JoystickSupport: unable to open device" << i;
			if (gamepad)
				SDL_GameControllerClose(gamepad);
			continue;
		}

		qDebug() << SDL_JoystickName(joystick) << "has:" << endl
		         << SDL_JoystickNumAxes(joystick) << "axes" << endl
		         << SDL_JoystickNumBalls(joystick) << "balls" << endl
		         << SDL_JoystickNumButtons(joystick) << "buttons" << endl
		         << SDL_JoystickNumHats(joystick) << "hats";

		if (gamepad)
			SDL_GameControllerClose(gamepad);
		else
			SDL_JoystickClose(joystick);
	}
}

void
JoystickSupport::handleJoystickButtons(StelCore* core)
{
	//
}

void
JoystickSupport::handleJoystickHats(StelCore* core)
{
	Q_ASSERT(joystick);
	Q_ASSERT(core);
	StelMovementMgr* movement = core->getMovementMgr();
	for (int i = 0; i < SDL_JoystickNumHats(joystick); i++)
	{
		Uint8 state = SDL_JoystickGetHat(joystick, i);
		switch (state)
		{
		case SDL_HAT_UP:
			movement->turnUp(true);
			break;
		case SDL_HAT_DOWN:
			movement->turnDown(true);
			break;
		case SDL_HAT_LEFT:
			movement->turnLeft(true);
			break;
		case SDL_HAT_RIGHT:
			movement->turnRight(true);
			break;
		case SDL_HAT_LEFTUP:
			movement->turnUp(true);
			movement->turnLeft(true);
			break;
		case SDL_HAT_LEFTDOWN:
			movement->turnDown(true);
			movement->turnLeft(true);
			break;
		case SDL_HAT_RIGHTUP:
			movement->turnUp(true);
			movement->turnRight(true);
			break;
		case SDL_HAT_RIGHTDOWN:
			movement->turnDown(true);
			movement->turnRight(true);
			break;
		case SDL_HAT_CENTERED:
		default:
			movement->turnUp(false);
			movement->turnDown(false);
			movement->turnLeft(false);
			movement->turnRight(false);
		}
	}
}
