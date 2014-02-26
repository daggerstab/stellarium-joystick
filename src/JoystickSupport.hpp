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

#include <QObject>

#include "StelModule.hpp"

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

private:
	bool initialized;
};


#include "StelPluginInterface.hpp"

class JoystickPluginInterface : public QObject, StelPluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "stellarium.StelPluginInterface/2.0")
	Q_INTERFACES(StelPluginInterface)

public:
	virtual StelModule* getStelModule() const;
	virtual StelPluginInfo getPluginInfo() const;
};

#endif//JOYSTICK_MODULE_HPP
