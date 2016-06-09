#pragma once
#include <boost/test/unit_test.hpp>
#include "src/machine/armmachine.hpp"
#include "src/machine/memory.hpp"
#include <vector>


struct ArmProgramFixture
{
	static constexpr auto STEP_LIMIT = 10000;

	Machine::Arm::Machine machine;
	Machine::Arm::Machine snapshot;

	Machine::Arm::regval r(int which)
	{
		return machine.cpu().regs()[which];
	}

	void r(int which, Machine::Arm::regval val)
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
		machine = Machine::Arm::Machine();
		auto addr = 0;
		for (auto it = program.cbegin(); it != program.cend(); ++it, addr += sizeof(*it))
		{
			machine.memory().putWord(addr, *it);
		}
		machine.cpu().regs().pc() = 0;
		machine.cpu().regs().lr() = addr + Machine::Arm::PREFETCH_SIZE;
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
