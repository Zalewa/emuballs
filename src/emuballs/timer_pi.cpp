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
#include "timer_pi.hpp"

#include "memory_impl.hpp"
#include <algorithm>
#include <chrono>

using namespace Emuballs;
using namespace Emuballs::Pi;
using Clock = std::chrono::steady_clock;
using Timepoint = Clock::time_point;
using Resolution = std::chrono::duration<uint64_t, std::ratio<1, 1000000>>;

namespace Emuballs
{
namespace Pi
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

DClass<Emuballs::Pi::Timer>
{
public:
	static const memsize INVALID_ADDRESS = -1;

	memsize address = INVALID_ADDRESS;
	Timepoint startingPoint;
	Memory *memory;

	bool isInit() const
	{
		return startingPoint.time_since_epoch() != Resolution::zero();
	}

	void init()
	{
		if (address == INVALID_ADDRESS)
			throw std::logic_error("timer has no address specified");
		startingPoint = Clock::now();
	}

	Timebox readTimebox()
	{
		std::vector<uint8_t> raw = memory->chunk(address, sizeof(Timebox));
		Timebox timebox;
		std::copy(raw.begin(), raw.end(), reinterpret_cast<uint8_t*>(&timebox));
		return timebox;
	}

	void writeTimebox(const Timebox &timebox)
	{
		std::vector<uint8_t> raw;
		raw.resize(sizeof(Timebox));
		std::copy(
			reinterpret_cast<const uint8_t*>(&timebox),
			reinterpret_cast<const uint8_t*>(&timebox + 1),
			raw.begin());
		memory->putChunk(address, raw);
	}
};

DPointeredNoCopy(Emuballs::Pi::Timer);

}

Timer::Timer(Memory &memory)
{
	d->memory = &memory;
}

void Timer::cycle()
{
	if (!d->isInit())
		d->init();

	Timepoint now = Clock::now();
	Resolution duration = std::chrono::duration_cast<Resolution>(now - d->startingPoint);
	Timebox timebox = d->readTimebox();
	timebox.counter = duration.count();
	d->writeTimebox(timebox);
}

void Timer::setAddress(memsize address)
{
	if (d->isInit())
		throw std::logic_error("cannot change timer address after init");
	d->address = address;
}