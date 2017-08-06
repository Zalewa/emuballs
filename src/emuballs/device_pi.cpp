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
#include "timer_pi.hpp"
#include <memory>

using namespace Emuballs;
using namespace Emuballs::Pi;

PiDef::PiDef()
{
	gpioCount = 26;
	actLedGpio = 0;
	ledOnIfPullDown = false;
	gpioAddress = 0;
	gpuFrameBufferPointerEnd = 0x20000000;
	gpuMailboxAddress = 0x2000B880;
	systemTimerAddress = 0x20003000;
}

namespace Emuballs
{
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
	}
};

DPointeredNoCopy(PiDevice);
}

PiDevice::PiDevice(const PiDef &definition)
{
	d->definition = definition;
	reset();
}

void PiDevice::cycle(uint32_t cycles)
{
	for (uint32_t i = 0; i < cycles; ++i)
	{
		d->machine.cycle();
		d->gpu->cycle();
	}
}

void PiDevice::draw(Canvas &canvas)
{
	d->gpu->draw(canvas);
}

Memory &PiDevice::memory()
{
	return d->machine.untrackedMemory();
}

void PiDevice::reset()
{
	d->gpu.reset(new Arm::Gpu(d->machine.untrackedMemory()));
	d->gpu->setFrameBufferPointerEnd(d->definition.gpuFrameBufferPointerEnd);
	d->gpu->setMailboxAddress(d->definition.gpuMailboxAddress);

	d->timer.reset(new Emuballs::Pi::Timer(
			d->machine.untrackedMemory(),
			d->definition.systemTimerAddress));

	d->regs.reset(new Arm::NamedRegisterSet(d->machine));
	d->machine.cpu().regs().pc(0x8000);
	setProgrammer(std::shared_ptr<Programmer>(new ProgrammerPi(*this)));
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
		DeviceFactory("Raspberry Pi 1", rpi1),
		DeviceFactory("Raspberry Pi 1 B+", rpi1b),
		DeviceFactory("Raspberry Pi 2", rpi2),
	};
	return factories;
}
