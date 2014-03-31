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

#ifndef JOYSTICK_MODULE_HPP
#define JOYSTICK_MODULE_HPP

#ifndef SDL_MAIN_HANDLED
#define SDL_MAIN_HANDLED
#endif
#include "SDL.h"

#include <QObject>
#include <QVector>

#include "StelModule.hpp"

class StelCore;
class StelMovementMgr;

//! Main class of the Joystick Support plug-in.
//!
//! As the name suggests, it is intended to add another way of control to
//! Stellarium, recognizing connected joystick devices and translating
//! button presses, axis moves and other events into the appropriate Stellarium
//! actions.
//!
//! For now this includes only panning and zooming the view,
//! making movements more precise/slow and switching between mount modes.
class JoystickSupport : public StelModule
{
	Q_OBJECT

public:
	JoystickSupport();
	~JoystickSupport();

	virtual void init();
	virtual void deinit();
	virtual void update(double deltaTime);
	virtual bool configureGui(bool show);

	//! Loads the SDL game controller database if such file exists.
	//! The file is expexted to be called "gamecontrollerdb.txt" and to be found
	//! in the modules/JoystickSupport/ sub-directory - either of Stellarium's
	//! installation directory, or the user data directory.
	//! @returns true if the file was found successfully, even if parsing it
	//! failed.
	bool loadGamepadDatabase();

private:
	//! Lists all connected devices and their properties in the log.
	//! Mostly a debugging function.
	void printDeviceDescriptions();

	//! Makes the selected device the currently active device.
	//! Populates the necessary fields. If a device is active, it's closed,
	//! even if it's the same one. If the device cannot be opened,
	//! #activeJoystick is set to null.
	//! @param deviceIndex is the logical device index as used in SDL.
	bool openDevice(int deviceIndex);
	//! Closes the currently active device, e.g. because it's disconnected.
	void closeDevice();

	//! Reads the current state of joystick axes and acts accordingly.
	//! Requires an open device in #activeJoystick.
	void handleJoystickAxes(StelCore* core);
	//! Reads the current state of joystick balls and acts accordingly.
	//! @warning Not implemented, as I have no way to test it.
	void handleJoystickBalls(StelCore* core);
	//! Reads the current state of joystick buttons and acts accordingly.
	//! Requires an open device in #activeJoystick.
	void handleJoystickButtons(StelCore* core);
	//! Reads the current state of joystick hat switches and acts accordingly.
	//! Gamepad direction buttons (the up/down/left/right quartet) are
	//! often interpreted as hat switches.
	//! Requires an open device in #activeJoystick.
	void handleJoystickHats(StelCore* core);

	//! Reads the current state of gamepad controlls and acts accordingly.
	//! @warning Calling this is mutually exclusive with the above functions.
	//! As a result, joystick trackballs won't be handled natively.
	// NOTE: Temporary, until I figure out how to add my own layer of bindings.
	void handleGamepad(StelCore* core);

	//! Helper function reading the state of gamepad buttons.
	void getButtonStateChange(const SDL_GameControllerButton& button,
	                          bool& state,
	                          bool& changed);

	//! Interprets an axis value as indicating horizontal movement direction.
	//! This means azimuth or right ascension depending on the mount mode.
	//! Negative is left (counterclockwise), positivive is right (clockwise).
	void interpretAsHorizontalMovement(StelMovementMgr* movement,
	                                   const Sint16& xAxis);
	//! Interprets an axis value as indicating vertical movement direction.
	//! This means altitude or declination depending on the mount mode.
	//! Negative is "up", positive is "down".
	void interpretAsVerticalMovement(StelMovementMgr* movement,
	                                 const Sint16& yAxis);
	//! Interprets an axis value as indicating zooming direction (in or out).
	void interpretAsZooming(StelMovementMgr* movement, const Sint16& zoomAxis);

	//! True if SDL was initialized correctly, if not - disables the plugin.
	bool initialized;

	// Temporary flag - prevents repeated output of device descriptions.
	bool devicesDescribed;

	//! The current active device, null if none is opened.
	SDL_Joystick* activeJoystick;
	//! The currently active gamepad, null if the active device is not one.
	//! A value implies that #activeJoystick is not null and contains
	//! the underlying joystick device.
	SDL_GameController* activeGamepad;

	//! For now, threshold/deadzone for all joystick axes.
	//! Axis values in the interval [-threshold;threshold] are ignored.
	Sint16 axisThreshold;

	//! State of the hat(s) on the previous update.
	QVector<Uint8> hatStates;
	//! State of the button(s) on the previous update.
	QVector<bool> buttonStates;
	//! State of the gamepad buttons on the previous update.
	// NOTE: Temporary. It would be easier to remove later.
	QVector<bool> gamepadStates;
};


#include "StelPluginInterface.hpp"

class JoystickPluginInterface : public QObject, StelPluginInterface
{
	Q_OBJECT
#if QT_VERSION >= 0x050000
	Q_PLUGIN_METADATA(IID "stellarium.StelPluginInterface/2.0")
#endif
	Q_INTERFACES(StelPluginInterface)

public:
	virtual StelModule* getStelModule() const;
	virtual StelPluginInfo getPluginInfo() const;
};

#endif//JOYSTICK_MODULE_HPP
