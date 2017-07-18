/*
 * Copyright 2017 Zalewa <zalewapl@gmail.com>.
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
#include "pi_timer.hpp"

#include "memory.hpp"
#include <chrono>

namespace Emuballs::Pi
{

struct Timebox
{
	static const auto NUM_COMPARES = 4;

	// TODO: How do `control` and `compare` work exactly?
	uint32_t control;
	uint64_t counter;
	uint32_t compare[NUM_COMPARES];
};

}

using namespace Emuballs;
using namespace Emuballs::Pi;
using Clock = std::chrono::steady_clock;
using Timepoint = Clock::time_point;
using Resolution = std::chrono::duration<uint64_t, std::ratio<1, 1000000>>;

DClass<Emuballs::Pi::Timer>
{
public:
	memsize address = 0x20003000;
	Timepoint startingPoint;
	Memory *memory;

	Timebox readTimebox()
	{
		MemoryStreamReader reader(*memory, address);
		Timebox timebox;
		timebox.control = reader.readUint32();
		timebox.counter = reader.readUint64();
		for (uint32_t &compare : timebox.compare)
			compare = reader.readUint32();
		return timebox;
	}

	void writeTimebox(const Timebox &timebox)
	{
		MemoryStreamWriter writer(*memory, address);
		writer.writeUint32(timebox.control);
		writer.writeUint64(timebox.counter);
		for (uint32_t compare : timebox.compare)
			writer.writeUint32(compare);
	}
};

DPointeredNoCopy(Emuballs::Pi::Timer);

Timer::Timer(Memory &memory)
{
	d->memory = &memory;
}

void Timer::cycle()
{
	Timepoint now = Clock::now();
	if (d->startingPoint.time_since_epoch() == Resolution::zero())
		d->startingPoint = now;

	Resolution duration = std::chrono::duration_cast<Resolution>(now - d->startingPoint);
	Timebox timebox = d->readTimebox();
	timebox.counter = duration.count();
	d->writeTimebox(timebox);
}
