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

#include "device.hpp"
#include "pickdevice.hpp"

#include "ui_mainwindow.h"

#include <QAction>
#include <QApplication>
#include <QList>
#include <QMessageBox>
#include <QTimer>
#include <emuballs/device.hpp>
#include <memory>

using namespace Emulens;

DClass<MainWindow> : public Ui::MainWindow
{
public:
	QList<QAction*> windowActions;
	Emulens::Device *deviceLens;
};

DPointeredNoCopy(MainWindow)

MainWindow::MainWindow()
{
	d->setupUi(this);

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
		if (d->deviceLens != nullptr)
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

void MainWindow::showLoadProgram()
{
	d->deviceLens->showLoadProgram();
}

void MainWindow::switchToDevice(Emuballs::DeviceFactory &factory)
{
	Emuballs::DevicePtr device = factory.create();
	if (device == nullptr)
	{
		QMessageBox::critical(this, tr("Emulens"), tr("This device is invalid."));
		return;
	}
	if (d->deviceLens)
		delete d->deviceLens;
	d->deviceLens = new Emulens::Device(std::shared_ptr<Emuballs::Device>(device.release()), this);
	addToolBar(d->deviceLens->toolBar());
	d->centralwidget->layout()->addWidget(d->deviceLens);
	updateWindowsList();
}

void MainWindow::cascadeSubWindows()
{
	if (d->deviceLens)
		d->deviceLens->cascadeSubWindows();
}

void MainWindow::closeAllSubWindows()
{
	if (d->deviceLens)
		d->deviceLens->closeAllSubWindows();
}

void MainWindow::tileSubWindows()
{
	if (d->deviceLens)
		d->deviceLens->tileSubWindows();
}

void MainWindow::updateWindowsList()
{
	d->windowActions.clear();
	if (d->deviceLens)
	{
		for (QAction *action : d->deviceLens->windowActions())
		{
			d->menuWindow->addAction(action);
			d->windowActions << action;
		}
	}
}
