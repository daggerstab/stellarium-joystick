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




JoystickSupport::JoystickSupport()
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
	// TODO: When is this called?
}

void
JoystickSupport::deinit()
{
	//
}

void
JoystickSupport::update(double deltaTime)
{
	//
	Q_UNUSED(deltaTime);
}

bool
JoystickSupport::configureGui(bool show)
{
	// TODO: Think of a way to make GUI more independent for all plugins.
	Q_UNUSED(show);
	return false; // For now there's no configuration window
}
