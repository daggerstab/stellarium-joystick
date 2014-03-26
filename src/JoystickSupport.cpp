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

#include <QDebug>

#ifndef SDL_MAIN_HANDLED
#define SDL_MAIN_HANDLED
#endif
#include "SDL.h"

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
	info.version = "0.0.1"; // TODO: Handle version numbers
	return info;
}




JoystickSupport::JoystickSupport() : initialized(false)
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
}

void
JoystickSupport::deinit()
{
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
	qDebug() << "JoystickSupport: Number of connected devices:" << deviceCount;
	// TODO: Emit signal if there is a change in connected number?

	// TODO: Update joystick/gamepad state?

	if (!devicesDescribed)
	{
		devicesDescribed = true;
		printDeviceDescriptions();
	}
}

bool
JoystickSupport::configureGui(bool show)
{
	// TODO: Think of a way to make GUI more independent for all plugins.
	Q_UNUSED(show);
	return false; // For now there's no configuration window
}

void JoystickSupport::printDeviceDescriptions()
{
	for (int i = 0; i < SDL_NumJoysticks(); i++) // Thread safety?
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
