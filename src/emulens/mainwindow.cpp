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
#include "mainwindow.hpp"

#include "ui_mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QTimer>
#include <emuballs/device.hpp>
#include "device.hpp"
#include "pickdevice.hpp"

using namespace Emulens;

DClass<MainWindow> : public Ui::MainWindow
{
public:
	Emuballs::DevicePtr device;
	Emulens::Device *deviceLens;
};

DPointeredNoCopy(MainWindow)

MainWindow::MainWindow()
{
	d->setupUi(this);

	d->device = nullptr;
	d->deviceLens = nullptr;

	connect(d->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::start()
{
	QTimer::singleShot(0, this, SLOT(showDeviceInitSelect()));
}

void MainWindow::showDeviceInitSelect()
{
	while (showDeviceChange())
	{
		if (d->device != nullptr)
		{
			show();
			return;
		}
	}
	close();
}

bool MainWindow::showDeviceChange()
{
	Emulens::PickDevice dialog;
	if (dialog.exec() == QDialog::Accepted)
	{
		Emuballs::DeviceFactory deviceFactory = dialog.pickedDeviceFactory();
		switchToDevice(deviceFactory);
		return true;
	}
	return false;
}

void MainWindow::switchToDevice(Emuballs::DeviceFactory &factory)
{
	d->device = factory.create();
	if (d->device == nullptr)
		QMessageBox::critical(this, tr("Emulens"), tr("This device is invalid."));
	if (d->deviceLens)
		delete d->deviceLens;
	d->deviceLens = new Emulens::Device(this);
	d->centralwidget->layout()->addWidget(d->deviceLens);
}
