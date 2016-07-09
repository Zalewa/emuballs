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

#include "device.hpp"
#include "dptr.hpp"

namespace Emuballs
{

namespace Devs
{

struct PiDef
{
	int gpioCount;
	int actLedGpio;
	bool ledOnIfPullDown;
	uint32_t gpioAddress;
	uint32_t gpuMailboxAddress;
	uint32_t systemTimerAddress;

	PiDef();
};

class Pi : public Device
{
public:
	Pi(const PiDef &definition);

	void cycle();
	void draw(Canvas &canvas);
	Memory &memory();
	RegisterSet &registers();

private:
	DPtr<Pi> d;
};

std::list<DeviceFactory> listPiDevices();

}

}