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
#include <boost/test/unit_test.hpp>
#include "src/emuballs/armmachine.hpp"
#include "src/emuballs/memory_impl.hpp"
#include <vector>


struct ArmProgramFixture
{
	static constexpr auto STEP_LIMIT = 10000;

	Emuballs::Arm::Machine machine;
	Emuballs::Arm::Machine snapshot;

	Emuballs::Arm::regval r(int which)
	{
		return machine.cpu().regs()[which];
	}

	void r(int which, Emuballs::Arm::regval val)
	{
		machine.cpu().regs()[which] = val;
	}

	auto &flags()
	{
		return machine.cpu().flags();
	}

	void load(const uint32_t *begin, const uint32_t *end)
	{
		std::vector<uint32_t> program(begin, end);
		load(program);
	}

	void load(const std::vector<uint32_t> &program)
	{
		machine = Emuballs::Arm::Machine();
		auto addr = 0;
		for (auto it = program.cbegin(); it != program.cend(); ++it, addr += sizeof(*it))
		{
			machine.memory().putWord(addr, *it);
		}
		machine.cpu().regs().pc() = 0;
		machine.cpu().regs().lr() = addr + Emuballs::Arm::PREFETCH_SIZE;
		snapshot = machine;
	}

	void reset()
	{
		machine = snapshot;
	}

	void runProgram()
	{
		auto endAddress = machine.cpu().regs().lr();
		auto limit = STEP_LIMIT;
		for (; limit > 0 && machine.cpu().regs().pc() < endAddress; --limit)
		{
			machine.cycle();
		}
		BOOST_WARN_EQUAL(machine.cpu().regs().pc(), endAddress);
		BOOST_REQUIRE_NE(0, limit);
	}
};
