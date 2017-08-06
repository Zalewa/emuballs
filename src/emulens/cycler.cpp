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
#include "cycler.hpp"

#include <emuballs/errors.hpp>
#include <QTimer>

using namespace Emulens;

DClass<Cycler>
{
public:
	QTimer cycleTimer;
	int deviceCyclesPerAutoRunCycle = 10000;
	std::shared_ptr<Emuballs::Device> device;
};

DPointeredNoCopy(Cycler);

Cycler::Cycler(std::shared_ptr<Emuballs::Device> device, QObject *parent)
	: QObject(parent)
{
	d->device = device;
	this->connect(&d->cycleTimer, &QTimer::timeout, this, &Cycler::cycle);
}

void Cycler::cycle()
{
	try
	{
		if (isAutoRun())
		{
			d->device->cycle(d->deviceCyclesPerAutoRunCycle);
		}
		else
		{
			d->device->cycle(1);
		}
		emit updated();
	}
	catch (const Emuballs::ProgramRuntimeError &e)
	{
		pauseAutoRun();
		emit error(QString("%1").arg(e.what()));
		emit updated();
	}
}

bool Cycler::isAutoRun() const
{
	return d->cycleTimer.isActive();
}

void Cycler::pauseAutoRun()
{
	d->cycleTimer.stop();
}

void Cycler::startAutoRun()
{
	d->cycleTimer.start();
}
