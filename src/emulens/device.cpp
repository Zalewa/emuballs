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

#include "ui_device.h"
#include <QMap>
#include <QMdiSubWindow>
#include <QTimer>

using namespace Emulens;

DClass<Device> : public Ui::Device
{
public:
	QMap<QAction*, QMdiSubWindow*> actions;
	Registers *registers;
};

DPointered(Device);

Device::Device(QWidget *parent)
	: QMdiArea(parent)
{
	d->setupUi(this);

	d->registers = new Registers(this);
	addSubWindow(d->registers);

	connect(this, &QMdiArea::subWindowActivated, this, &Device::updateActiveWindowAction);

	QTimer::singleShot(0, this, &Device::updateActiveWindowAction);
}

void Device::addSubWindow(QWidget *widget)
{
	auto *window = new QMdiSubWindow(this);
	window->setWidget(widget);

	auto *action = new QAction(window->windowTitle(), this);
	action->setCheckable(true);
	connect(action, &QAction::triggered, window, [=]() {
			window->show();
			window->widget()->show();
			setActiveSubWindow(window);
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
