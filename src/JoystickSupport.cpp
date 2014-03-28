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



JoystickSupport::JoystickSupport() :
    initialized(false),
    activeJoystick(NULL),
    activeGamepad(NULL)
{
	setObjectName("JoystickSupport");

	// TODO: This should be read from the configuration file.
	// Ultimately, set separately for all axes, individually and/or in pairs.
	axisThreshold = (2 << 13); // Around 8000
	// Movement is between -32768 and 32767, so this is about one quarter.
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
	if (activeJoystick)
	{
		closeDevice();
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
		if (activeJoystick)
		{
			closeDevice();
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
	if (activeJoystick == NULL)
	{
		openDevice(index);
	}

	// Prevent attempts at reading from a detached device.
	if (SDL_JoystickGetAttached(activeJoystick) == SDL_FALSE)
	{
		closeDevice();
		qWarning() << "Joystick Support: device" << index << "disconnected?";
	}

	if (activeJoystick)
	{
		SDL_JoystickUpdate();
		//SDL_GameControllerUpdate();

		StelCore* core = StelApp::getInstance().getCore();
		// FIXME: Movement may depend on the order these are called. Fixed for hats?
		handleJoystickAxes(core);
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

bool
JoystickSupport::openDevice(int deviceIndex)
{
	if (activeJoystick)
		closeDevice();

	if (SDL_IsGameController(deviceIndex))
	{
		activeGamepad = SDL_GameControllerOpen(deviceIndex);
		activeJoystick = SDL_GameControllerGetJoystick(activeGamepad);
		if (activeJoystick == NULL)
		{
			SDL_GameControllerClose(activeGamepad);
			activeGamepad = NULL;
		}
	}
	else
		activeJoystick = SDL_JoystickOpen(deviceIndex);

	if (activeJoystick == NULL)
	{
		qWarning() << "JoystickSupport: unable to open device" << deviceIndex
		           << SDL_GetError();
		return false;
	}

	// Initialize various "previous state" holders
	hatStates.fill(SDL_HAT_CENTERED, SDL_JoystickNumHats(activeJoystick));
	buttonStates.fill(false, SDL_JoystickNumButtons(activeJoystick));

	return true;
}

void
JoystickSupport::closeDevice()
{
	if (activeGamepad)
	{
		SDL_GameControllerClose(activeGamepad);
		activeGamepad = NULL;
	}
	else if (activeJoystick)
		SDL_JoystickClose(activeJoystick);

	activeJoystick = NULL;
}

void JoystickSupport::handleJoystickAxes(StelCore* core)
{
	Q_ASSERT(activeJoystick);
	Q_ASSERT(core);

	int axesCount = SDL_JoystickNumAxes(activeJoystick);
	if (axesCount < 1)
		return;
	QVector<Sint16> axisValues(axesCount, 0);
	for (int i = 0; i < axesCount; i++)
	{
		axisValues[i] = SDL_JoystickGetAxis(activeJoystick, i);
	}

	if (axesCount == 1) // Some kind of paddle?
	{
		interpretAsZooming(core, axisValues[0]);
		return;
	}

	if (axesCount >= 2) // Two axes, assuming 0==X, 1==Y, negative is left/up.
	{
		interpretAsHorizontalMovement(core, axisValues[0]);
		interpretAsVerticalMovement(core, axisValues[1]);
	}

	if (axesCount >= 3) // Third axis is assumed to be a throttle.
		interpretAsZooming(core, axisValues[2]);
}

void
JoystickSupport::handleJoystickButtons(StelCore* core)
{
	Q_ASSERT(activeJoystick);
	Q_ASSERT(core);
	StelMovementMgr* movement = core->getMovementMgr();

	for (int i = 0; i < buttonStates.count(); i++)
	{
		bool state = (SDL_JoystickGetButton(activeJoystick, i) == 1);
		bool prevState = buttonStates[i];

		// Some buttons trigger one-time events, others control a state.
		switch (i)
		{
		case 0: // Triggers mounting change
			if (state != prevState)
			{
				if (!state) // Released after pressing
					movement->toggleMountMode();
			}
			break;
		case 1: // Slow movement mode
			movement->moveSlow(state);
			break;
		default:
			break;
		}

		buttonStates[i] = state;
	}
}

void
JoystickSupport::handleJoystickHats(StelCore* core)
{
	Q_ASSERT(activeJoystick);
	Q_ASSERT(core);
	StelMovementMgr* movement = core->getMovementMgr();
	for (int i = 0; i < hatStates.count(); i++)
	{
		Uint8 state = SDL_JoystickGetHat(activeJoystick, i);

		// This results in discrete movement (one button press, one step) :))
//		if (hatStates[i] == state)
//			continue;

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
			// Only clear the flags if the hat has changed them previously
			// (otherwise this overwrites changes made by e.g. axis movements)
			if (hatStates[i] == state)
				continue;
			movement->turnUp(false);
			movement->turnDown(false);
			movement->turnLeft(false);
			movement->turnRight(false);
		}

		hatStates[i] = state;
	}
}

void
JoystickSupport::interpretAsHorizontalMovement(StelCore* core,
                                               const Sint16& xAxis)
{
	StelMovementMgr* movement = core->getMovementMgr();

	if (xAxis < -axisThreshold)
		movement->turnLeft(true);
	else if (xAxis > axisThreshold)
		movement->turnRight(true);
	else
	{
		movement->turnLeft(false);
		movement->turnRight(false);
	}
}

void JoystickSupport::interpretAsVerticalMovement(StelCore* core,
                                                  const Sint16& yAxis)
{
	StelMovementMgr* movement = core->getMovementMgr();

	if (yAxis < -axisThreshold)
		movement->turnUp(true);
	else if (yAxis > axisThreshold)
		movement->turnDown(true);
	else
	{
		movement->turnUp(false);
		movement->turnDown(false);
	}
}

void
JoystickSupport::interpretAsZooming(StelCore* core, const Sint16& zoomAxis)
{
	StelMovementMgr* movement = core->getMovementMgr();

	if (zoomAxis < -axisThreshold)
		movement->zoomIn(true);
	else if (zoomAxis > axisThreshold)
		movement->zoomOut(true);
	else
	{
		movement->zoomIn(false);
		movement->zoomOut(false);
	}
}
