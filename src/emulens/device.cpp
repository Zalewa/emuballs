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
#include "device.hpp"

#include "registers.hpp"

#include <emuballs/device.hpp>
#include <emuballs/errors.hpp>
#include <emuballs/programmer.hpp>
#include "trackablemdiwindow.hpp"
#include "ui_device.h"
#include <fstream>
#include <QFileInfo>
#include <QMap>
#include <QMessageBox>
#include <QTimer>

namespace Emulens
{
static const auto SANE_FILE_SIZE = 64 * 1024 * 1024;
static const QString SANE_FILE_SIZE_HUMAN_READABLE = "64 MB";
}

using namespace Emulens;


DClass<Emulens::Device> : public Ui::Device
{
public:
	QMap<QAction*, QMdiSubWindow*> actions;
	Registers *registers;
	std::shared_ptr<Emuballs::Device> device;
};

DPointeredNoCopy(Device);

Device::Device(std::shared_ptr<Emuballs::Device> device, QWidget *parent)
	: QMdiArea(parent)
{
	d->setupUi(this);

	d->device = device;
	d->registers = new Registers(device, this);
	addSubWindow(d->registers);

	connect(this, &QMdiArea::subWindowActivated, this, &Device::updateActiveWindowAction);
	connect(this, &QMdiArea::subWindowActivated, this, &Device::updateActiveWindowAction);

	QTimer::singleShot(0, this, &Device::updateActiveWindowAction);
}

void Device::addSubWindow(QWidget *widget)
{
	auto *window = new TrackableMdiWindow(this);
	window->setWidget(widget);

	connect(window, &TrackableMdiWindow::stateChanged, this, &Device::updateActiveWindowAction);

	auto *action = new QAction(window->windowTitle(), this);
	action->setCheckable(true);
	connect(action, &QAction::triggered, window, [=]() {
			window->show();
			window->widget()->show();
			setActiveSubWindow(window);
			updateActiveWindowAction();
		});

	int ordinal = d->actions.size() + 1;
	if (ordinal <= 9)
	{
		action->setShortcut(QKeySequence(QString("ALT+%1").arg(ordinal),
				QKeySequence::PortableText));
	}
	++ordinal;

	d->actions.insert(action, window);
	QMdiArea::addSubWindow(window);
}

void Device::loadProgram(const QString &path)
{
	std::ifstream stream(path.toStdString(), std::ios::in | std::ios::binary);
	if (!stream.is_open())
	{
		QMessageBox::critical(this, tr("Load Program Error"),
			tr("Failed to open file '%1'.").arg(path));
		return;
	}

	QFileInfo file(path);
	if (checkProgramSize(file.size()))
	{
		try
		{
			d->device->programmer().load(stream);
		}
		catch (const Emuballs::ProgramLoadError &e)
		{
			QMessageBox::critical(this, tr("Load Program Error"),
				tr("Couldn't load program: %1").arg(e.what()));
		}
	}
}

bool Device::checkProgramSize(size_t size)
{
	if (size <= SANE_FILE_SIZE)
		return true;
	return QMessageBox::Yes == QMessageBox::question(this, tr("Load Program Warning"),
		tr("Trying to load a file larger than %1. Proceed?").arg(
			SANE_FILE_SIZE_HUMAN_READABLE));
}

void Device::updateActiveWindowAction()
{
	for (auto *action : d->actions.keys())
	{
		auto *window = d->actions[action];
		action->setChecked(window == activeSubWindow());
	}
}

QList<QAction*> Device::windowActions()
{
	return d->actions.keys();
}
