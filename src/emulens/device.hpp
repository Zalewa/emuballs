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

#include <emuballs/device.hpp>

#include <memory>
#include <QAction>
#include <QIODevice>
#include <QList>
#include <QMdiArea>
#include <QToolBar>
#include "dptr.hpp"

namespace Emulens
{

class TrackableMdiWindow;

class Device : public QMdiArea
{
	Q_OBJECT

public:
	Device(std::shared_ptr<Emuballs::Device> device, QWidget *parent);
	Device(const Device &other) = delete;
	Device &operator=(const Device &other) = delete;

	void loadProgram(const QString &path);
	QList<QAction*> windowActions();
	QToolBar *toolBar();

public slots:
	void showLoadProgram();

protected:
	virtual void showEvent(QShowEvent *event) override;

private:
	DPtr<Device> d;

	TrackableMdiWindow *addSubWindow(QWidget *widget);
	bool checkProgramSize(size_t size);
	void postInit();
	void setupCycler();
	void setupToolBar();

private slots:
	void restartLastProgram();
	void showProgramRuntimeError(const QString &error);
	void updateActiveWindowAction();
	void updateViews();
};

class DeviceToolBar : public QToolBar
{
	Q_OBJECT

	friend class Device;

public:
	DeviceToolBar(Device *device);

private:
	DPtr<DeviceToolBar> d;

	QAction *loadProgramAction;
	QAction *restartAction;
	QAction *startRunAction;
	QAction *pauseAction;
	QAction *stepAction;
};

}
