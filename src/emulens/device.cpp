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

#include "cycler.hpp"
#include "display.hpp"
#include "memory.hpp"
#include "registers.hpp"
#include "trackablemdiwindow.hpp"
#include "ui_device.h"

#include <emuballs/device.hpp>
#include <emuballs/errors.hpp>
#include <emuballs/programmer.hpp>
#include <fstream>
#include <QElapsedTimer>
#include <QFileDialog>
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
	std::unique_ptr<Cycler> cycler;
	Display *display;
	TrackableMdiWindow *displayWindow;
	QString lastLoadedProgramPath;
	Memory *memory;
	bool postInit;
	Registers *registers;
	std::shared_ptr<Emuballs::Device> device;
	std::unique_ptr<DeviceToolBar> toolBar;
	int refreshIntervalMs = 200;
	QElapsedTimer refreshClock;
};

DPointeredNoCopy(Device);

Device::Device(std::shared_ptr<Emuballs::Device> device, QWidget *parent)
	: QMdiArea(parent)
{
	d->setupUi(this);

	d->refreshClock.start();
	d->postInit = false;
	d->device = device;
	d->cycler.reset(new Cycler(device, this));
	d->registers = new Registers(device, this);
	d->memory = new Memory(device, this);
	d->display = new Display(device, this);

	setupToolBar();
	setupCycler();

	d->displayWindow = addSubWindow(d->display);
	addSubWindow(d->registers);
	addSubWindow(d->memory);

	connect(this, &QMdiArea::subWindowActivated, this, &Device::updateActiveWindowAction);

	QTimer::singleShot(0, this, &Device::updateActiveWindowAction);
}

TrackableMdiWindow *Device::addSubWindow(QWidget *widget)
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
	return window;
}

void Device::showProgramRuntimeError(const QString &error)
{
	QMessageBox::critical(this, tr("Program Runtime Error"), error);
}

void Device::showLoadProgram()
{
	QString filePath = QFileDialog::getOpenFileName(this, tr("Load Program"));
	if (!filePath.isEmpty())
		loadProgram(filePath);
}

void Device::restartLastProgram()
{
	if (!d->lastLoadedProgramPath.isEmpty())
	{
		loadProgram(d->lastLoadedProgramPath);
	}
	else
	{
		QMessageBox::critical(this, tr("Restart Program"),
			tr("No program was started, yet. Nothing to restart."));
	}
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
		d->device->reset();
		try
		{
			d->device->programmer().load(stream);
			d->lastLoadedProgramPath = path;
		}
		catch (const Emuballs::ProgramLoadError &e)
		{
			QMessageBox::critical(this, tr("Load Program Error"),
				tr("Couldn't load program: %1").arg(e.what()));
		}
		updateViews();
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

void Device::setupCycler()
{
	connect(d->cycler.get(), &Cycler::updated,
		this, &Device::updateViews);
	connect(d->cycler.get(), &Cycler::error,
		this, &Device::showProgramRuntimeError);
}

void Device::setupToolBar()
{
	d->toolBar.reset(new DeviceToolBar(this));
	connect(d->toolBar->loadProgramAction, &QAction::triggered,
		this, &Device::showLoadProgram);
	connect(d->toolBar->restartAction, &QAction::triggered,
		this, &Device::restartLastProgram);
	connect(d->toolBar->startRunAction, &QAction::triggered,
		d->cycler.get(), &Cycler::startAutoRun);
	connect(d->toolBar->pauseAction, &QAction::triggered,
		d->cycler.get(), &Cycler::pauseAutoRun);
	connect(d->toolBar->pauseAction, &QAction::triggered,
		this, &Device::updateViewsForce);
	connect(d->toolBar->stepAction, &QAction::triggered,
		d->cycler.get(), &Cycler::cycle);
}

void Device::showEvent(QShowEvent *event)
{
	if (!d->postInit)
	{
		postInit();
		d->postInit = true;
	}
	QMdiArea::showEvent(event);
}

void Device::postInit()
{
	d->displayWindow->resize(width(), height());
}

QToolBar *Device::toolBar()
{
	return d->toolBar.get();
}

void Device::updateActiveWindowAction()
{
	for (auto *action : d->actions.keys())
	{
		auto *window = d->actions[action];
		action->setChecked(window == activeSubWindow());
	}
}

void Device::updateViews()
{
	if (d->cycler->isAutoRun())
	{
		if (d->refreshClock.elapsed() < d->refreshIntervalMs)
			return;
		d->refreshClock.start();
	}
	updateViewsForce();
}

void Device::updateViewsForce()
{
	Updateable* views[] = { d->registers, d->memory, d->display };
	for (auto *view : views)
		view->update();
}

QList<QAction*> Device::windowActions()
{
	return d->actions.keys();
}

///////////////////////////////////////////////////////////////////////////

DClass<DeviceToolBar>
{
public:
	Device *device;
};

DPointeredNoCopy(DeviceToolBar);

DeviceToolBar::DeviceToolBar(Device *device)
	: QToolBar(device)
{
	d->device = device;

	loadProgramAction = addAction(tr("Load program"));
	restartAction = addAction(tr("Reset device"));
	addSeparator();
	startRunAction = addAction(tr("Run"));
	pauseAction = addAction(tr("Pause"));
	stepAction = addAction(tr("Step"));
}
