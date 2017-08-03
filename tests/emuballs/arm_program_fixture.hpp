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
#include "src/emuballs/memory.hpp"
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
		machine.cpu().regs().set(which, val);
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
		machine.cpu().regs().pc(0);
		machine.cpu().regs().lr(addr + Emuballs::Arm::PREFETCH_SIZE);
		snapshot = machine;
	}

	void reset()
	{
		machine = snapshot;
	}

	void runProgram(int limit = STEP_LIMIT)
	{
		auto endAddress = machine.cpu().regs().lr();
		for (; limit > 0 && machine.cpu().regs().pc() < endAddress; --limit)
		{
			machine.cycle();
		}
		BOOST_WARN_EQUAL(machine.cpu().regs().pc(), endAddress);
		BOOST_REQUIRE_NE(0, limit);
	}
};

constexpr uint32_t fibonacciCode[] = {
	// 00000000 <fibonacci>:
	0xe1a03000, // mov	r3, r0
	0xe3330000, // teq	r3, #0
	0x03a00000, // moveq	r0, #0
	0x01a0f00e, // moveq	pc, lr
	0xe3330001, // teq	r3, #1
	0x03a00001, // moveq	r0, #1
	0x01a0f00e, // moveq	pc, lr
	0xe3a01000, // mov	r1, #0
	0xe3a02001, // mov	r2, #1
	// 00000024 <_fib_loop>:
	0xe3330001, // teq	r3, #1
	0x01a0f00e, // moveq	pc, lr
	0xe0810002, // add	r0, r1, r2
	0xe1a01002, // mov	r1, r2
	0xe1a02000, // mov	r2, r0
	0xe2433001, // sub	r3, r3, #1
	0xeafffff8, // b	24 <_fib_loop>
};
