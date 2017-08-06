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
#pragma once

#include "emuballs/device.hpp"

#include "dptr_impl.hpp"

namespace Emuballs
{

namespace Pi
{

struct PiDef
{
	int gpioCount;
	int actLedGpio;
	bool ledOnIfPullDown;
	uint32_t gpioAddress;
	uint32_t gpuFrameBufferPointerEnd;
	uint32_t gpuMailboxAddress;
	uint32_t systemTimerAddress;

	PiDef();
};

class PiDevice : public Device
{
public:
	PiDevice(const PiDef &definition);

	void cycle(uint32_t cycles) override;
	void draw(Canvas &canvas) override;
	Memory &memory() override;
	void reset() override;
	RegisterSet &registers() override;

private:
	DPtr<PiDevice> d;
};

std::list<DeviceFactory> listPiDevices();

}

}
