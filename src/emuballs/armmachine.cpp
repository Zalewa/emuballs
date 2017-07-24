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
#include "armmachine.hpp"

#include "emuballs/errors.hpp"

#include "memory_impl.hpp"
#include "opdecoder.hpp"

#include <queue>
#include <sstream>

namespace Emuballs { namespace Arm {

RegisterSet::RegisterSet()
{
	regs.resize(16, 0);
}

auto RegisterSet::sp() -> regval&
{
	return (*this)[13];
}

auto RegisterSet::sp() const -> const regval&
{
	return (*this)[13];
}

auto RegisterSet::lr() -> regval&
{
	return (*this)[14];
}

auto RegisterSet::lr() const -> const regval&
{
	return (*this)[14];
}

auto RegisterSet::pc() -> regval&
{
	return (*this)[15];
}

auto RegisterSet::pc() const -> const regval&
{
	return (*this)[15];
}

auto RegisterSet::operator[](int idx) -> regval&
{
	return regs[idx];
}

auto RegisterSet::operator[](int idx) const -> const regval&
{
	return regs[idx];
}

}}

///////////////////////////////////////////////////////////////////////////

namespace Emuballs { namespace Arm {

regval Flags::dump() const
{
	return bits.to_ulong();
}

void Flags::store(regval bitset)
{
	bits = std::bitset<32>(bitset);
}

void Flags::set(Bit bit, bool state)
{
	bits.set(bit, state);
}

bool Flags::test(Bit bit) const
{
	return bits.test(bit);
}

bool Flags::carry() const
{
	return test(Carry);
}

bool Flags::carry(bool flag)
{
	set(Carry, flag);
	return flag;
}

bool Flags::overflow() const
{
	return test(Overflow);
}

bool Flags::overflow(bool flag)
{
	set(Overflow, flag);
	return flag;
}

bool Flags::zero() const
{
	return test(Zero);
}

bool Flags::zero(bool flag)
{
	set(Zero, flag);
	return flag;
}

bool Flags::negative() const
{
	return test(Negative);
}

bool Flags::negative(bool flag)
{
	set(Negative, flag);
	return flag;
}

cpumode Flags::cpuMode() const
{
	return dump() & 0x1f;
}


}}

///////////////////////////////////////////////////////////////////////////

namespace Emuballs
{

DClass<Emuballs::Arm::Cpu>
{
public:
	Emuballs::Arm::Flags flags;
	Emuballs::Arm::Flags storedFlags[32];
	Emuballs::Arm::RegisterSet regs;
};

DPointered(Emuballs::Arm::Cpu);

namespace Arm
{

const Flags &Cpu::flags() const
{
	return d->flags;
}

Flags &Cpu::flags()
{
	return d->flags;
}

const Flags &Cpu::flagsSpsr() const
{
	return flagsSpsr(flags().cpuMode());
}

Flags &Cpu::flagsSpsr()
{
	return flagsSpsr(flags().cpuMode());
}

const Flags &Cpu::flagsSpsr(cpumode mode) const
{
	if (mode >= 32)
		throw std::out_of_range("cpumode must be <= 31, was: " + std::to_string(mode));
	return d->storedFlags[mode];
}

Flags &Cpu::flagsSpsr(cpumode mode)
{
	return const_cast<Flags&>( static_cast<const Cpu&>(*this).flagsSpsr(mode) );
}

const RegisterSet &Cpu::regs() const
{
	return d->regs;
}

RegisterSet &Cpu::regs()
{
	return d->regs;
}

}}

///////////////////////////////////////////////////////////////////////////

namespace Emuballs { namespace Arm
{
class Prefetch
{
public:
	uint32_t next(Machine &machine)
	{
		collect(machine);
		return pop();
	}

private:
	std::queue<uint32_t> prefetchedInstructions;

	void collect(Machine &machine)
	{
		while (prefetchedInstructions.size() < 2)
		{
			auto &pc = machine.cpu().regs().pc();
			uint32_t instruction = machine.memory().word(pc);
			prefetchedInstructions.push(instruction);
			pc += 4;
		}
	}

	uint32_t pop()
	{
		auto instruction = prefetchedInstructions.front();
		prefetchedInstructions.pop();
		return instruction;
	}
};
}

DClass<Emuballs::Arm::Machine>
{
public:
	Emuballs::Arm::Cpu cpu;
	Emuballs::Memory memory;
	Emuballs::Arm::Prefetch prefetch;

	void flushPrefetch()
	{
		prefetch = Emuballs::Arm::Prefetch();
	}
};

DPointered(Emuballs::Arm::Machine);

}


Emuballs::Arm::Cpu &Emuballs::Arm::Machine::cpu()
{
	return d->cpu;
}

const Emuballs::Arm::Cpu &Emuballs::Arm::Machine::cpu() const
{
	return d->cpu;
}

Emuballs::TrackedMemory Emuballs::Arm::Machine::memory()
{
	return TrackedMemory(d->memory);
}

const Emuballs::TrackedMemory Emuballs::Arm::Machine::memory() const
{
	return TrackedMemory(d->memory);
}

Emuballs::Memory &Emuballs::Arm::Machine::untrackedMemory()
{
	return d->memory;
}

const Emuballs::Memory &Emuballs::Arm::Machine::untrackedMemory() const
{
	return d->memory;
}

void Emuballs::Arm::Machine::cycle()
{
	// Prefetch instructions and get next instruction to execute.
	auto instruction = d->prefetch.next(*this);
	auto pc = cpu().regs().pc();
	// Decode opcode.
	OpDecoder decoder;
	OpcodePtr opcode;
	try
	{
		opcode = decoder.decode(instruction);
	}
	catch (const OpDecodeError &error)
	{
		std::stringstream ss;
		ss << error.what() << "; instruction addr = 0x" << std::hex << pc - PREFETCH_SIZE;
		throw OpDecodeError(ss.str());
	}
	// Execute opcode.
	opcode->execute(*this);
	// Flush prefetched instructions if pc register changed due to opcode
	// execution.
	if (pc != cpu().regs().pc())
	{
		d->flushPrefetch();
	}
}
