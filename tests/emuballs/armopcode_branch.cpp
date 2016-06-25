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
#define BOOST_TEST_MODULE machine_armopcode_branch
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <string>

#include "src/emuballs/armopcode_impl.hpp"
#include "src/emuballs/armmachine.hpp"

using namespace Emuballs::Arm;

struct State
{
	Emuballs::Arm::Machine machine;
	Emuballs::Arm::Cpu &cpu;

	State() : cpu(machine.cpu())
	{
		cpu.regs().pc() = 0x80000000U;
		cpu.regs().lr() = 0xcafefeed;
	}
};

constexpr Emuballs::Arm::regval codes[] = {
	0xeafffffe,
	0xea000001,
	0xea7fffff,
	0xea800000,
	0xea000020,
	0xeaffffff - 0x20
};

static uint32_t offset(uint32_t code)
{
	auto masked = code & 0x00ffffff;
	auto sign = code & 0x00800000;
	uint32_t offset = (masked << 2) | (sign ? 0xfc000000 : 0);
	return offset;
}

static std::string hexval(uint32_t code, uint32_t actual, uint32_t expected, std::string comparison)
{
	std::stringstream ss;
	ss << std::endl << "\t" << std::hex << code << ": actual: " << actual
		<< " " << comparison << " expected: " << expected;
	return ss.str();
}

BOOST_AUTO_TEST_CASE(branch)
{
	for (auto code : codes)
	{
		State state;
		auto &regs = state.cpu.regs();
		auto pc = regs.pc();
		auto lr = regs.lr();
		auto op = opcodeBranch(code);
		auto pcExpected = pc + offset(code);
		op->execute(state.machine);
		BOOST_CHECK_MESSAGE(regs.pc() == pcExpected,
			hexval(code, regs.pc(), pcExpected, "=="));
		BOOST_CHECK_MESSAGE(regs.lr() == lr,
			hexval(code, regs.lr(), lr, "=="));
	}
}

BOOST_AUTO_TEST_CASE(branchWithLink)
{
	for (auto code : codes)
	{
		code |= 0x01000000;
		State state;
		auto &regs = state.cpu.regs();
		auto pc = regs.pc();
		auto lr = regs.lr();
		auto pcExpected = pc + offset(code);
		auto op = opcodeBranch(code);
		op->execute(state.machine);
		BOOST_CHECK_MESSAGE(regs.pc() == pcExpected,
			hexval(code, regs.pc(), pcExpected, "=="));
		BOOST_CHECK_MESSAGE(regs.lr() == pc - PREFETCH_SIZE + INSTRUCTION_SIZE,
			hexval(code, regs.lr(), pc - PREFETCH_SIZE + INSTRUCTION_SIZE, "=="));
		BOOST_CHECK_MESSAGE(regs.lr() != lr,
			hexval(code, regs.lr(), lr, "!="));
	}
}
