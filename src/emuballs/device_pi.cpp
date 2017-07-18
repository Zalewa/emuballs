/*
 * Copyright 2016 Zalewa <zalewapl@gmail.com>.
 *
 * This file is part of Emuballs.
 *
 * Emuballs is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Emuballs is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Emuballs.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "device_pi.hpp"

#include "armmachine.hpp"
#include "armregisterset.hpp"
#include "armgpu.hpp"
#include "programmer_pi.hpp"
#include "pi_timer.hpp"
#include <memory>

using namespace Emuballs;
using namespace Emuballs::Pi;

PiDef::PiDef()
{
	gpioCount = 26;
	actLedGpio = 0;
	ledOnIfPullDown = false;
	gpioAddress = 0;
	gpuMailboxAddress = 0x2000B880;
	systemTimerAddress = 0x20003000;
}

DClass<PiDevice>
{
public:
	Arm::Machine machine;
	std::unique_ptr<Arm::Gpu> gpu;
	std::unique_ptr<Arm::NamedRegisterSet> regs;
	std::unique_ptr<Pi::Timer> timer;
	PiDef definition;

	PrivData()
	{
		gpu.reset(new Arm::Gpu(machine.memory()));
		regs.reset(new Arm::NamedRegisterSet(machine));
		timer.reset(new Emuballs::Pi::Timer(machine.memory()));
	}
};

DPointeredNoCopy(PiDevice);

PiDevice::PiDevice(const PiDef &definition)
{
	d->definition = definition;
	d->gpu->setMailboxAddress(definition.gpuMailboxAddress);
	setProgrammer(std::shared_ptr<Programmer>(new ProgrammerPi(*this)));
}

void PiDevice::cycle()
{
	d->timer->cycle();
	d->machine.cycle();
	d->gpu->cycle();
}

void PiDevice::draw(Canvas &canvas)
{
	d->gpu->draw(canvas);
}

Memory &PiDevice::memory()
{
	return d->machine.memory();
}

void PiDevice::reset()
{
	d->machine.cpu().regs().pc() = 0;
}

RegisterSet &PiDevice::registers()
{
	return *d->regs;
}

///////////////////////////////////////////////////////////////////////////

std::list<DeviceFactory> Emuballs::Pi::listPiDevices()
{
	device_factory rpi1 = [](){
		PiDef def;
		def.gpioCount = 26;
		def.actLedGpio = 16;
		def.ledOnIfPullDown = true;
		def.gpioAddress = 0x20200000;
		return DevicePtr(new PiDevice(def));
	};

	device_factory rpi1b = [](){
		PiDef def;
		def.gpioCount = 40;
		def.actLedGpio = 47;
		def.ledOnIfPullDown = false;
		def.gpioAddress = 0x20200000;
		return DevicePtr(new PiDevice(def));
	};

	device_factory rpi2 = [](){
		PiDef def;
		def.gpioCount = 40;
		def.actLedGpio = 47;
		def.ledOnIfPullDown = false;
		def.gpioAddress = 0x3f200000;
		def.gpuMailboxAddress = 0x3f00b880;
		return DevicePtr(new PiDevice(def));
	};

	std::list<DeviceFactory> factories = {
		DeviceFactory(L"Raspberry Pi 1", rpi1),
		DeviceFactory(L"Raspberry Pi 1 B+", rpi1b),
		DeviceFactory(L"Raspberry Pi 2", rpi2),
	};
	return factories;
}
