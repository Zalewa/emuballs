/*
 * Copyright 2016 Zalewa <zalewapl@gmail.com>.
 *
 * This file is part of Emulens.
 *
 * Emulens is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Emulens is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Emulens.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once


#include "dptr.hpp"
#include <QMainWindow>

namespace Emuballs
{
class DeviceFactory;
}

namespace Emulens
{

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();

	void start();

private:
	DPtr<MainWindow> d;

	void switchToDevice(Emuballs::DeviceFactory &factory);
	void updateWindowsList();

private slots:
	void cascadeSubWindows();
	void closeAllSubWindows();
	void tileSubWindows();

	void showAbout();
	bool showDeviceChange();
	void showDeviceInitSelect();
	void showLoadProgram();
};

}
