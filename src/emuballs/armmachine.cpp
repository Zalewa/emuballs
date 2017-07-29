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

#include "array_queue.hpp"
#include "memory_impl.hpp"
#include "opdecoder.hpp"

#include <queue>
#include <sstream>

namespace Emuballs { namespace Arm {

RegisterSet::RegisterSet()
{
	regs.resize(16, 0);
	resetPcChanged();
}

auto RegisterSet::sp(const regval &value) -> void
{
	return set(13, value);
}

auto RegisterSet::sp() const -> const regval&
{
	return (*this)[13];
}

auto RegisterSet::lr(const regval &value) -> void
{
	return set(14, value);
}

auto RegisterSet::lr() const -> const regval&
{
	return (*this)[14];
}

auto RegisterSet::pc(const regval &value) -> void
{
	return set(15, value);
}

auto RegisterSet::pc() const -> const regval&
{
	return (*this)[15];
}

auto RegisterSet::set(int idx, const regval &value) -> void
{
	if (idx == 15)
		pcChanged = true;
	regs[idx] = value;
}

auto RegisterSet::operator[](int idx) const -> const regval&
{
	return regs[idx];
}

void RegisterSet::resetPcChanged()
{
	pcChanged = false;
}

bool RegisterSet::wasPcChanged() const
{
	return pcChanged;
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

	void flush()
	{
		prefetchedInstructions.clear();
	}

private:
	ArrayQueue<uint32_t, PREFETCH_INSTRUCTIONS> prefetchedInstructions;

	void collect(Machine &machine)
	{
		while (prefetchedInstructions.size() < PREFETCH_INSTRUCTIONS)
		{
			regval pc = machine.cpu().regs().pc();
			uint32_t instruction = machine.untrackedMemory().word(pc);
			prefetchedInstructions.push(instruction);
			machine.cpu().regs().pc(pc + 4);
		}
	}

	uint32_t pop()
	{
		auto instruction = prefetchedInstructions.pop();
		return instruction;
	}
};
}

DClass<Emuballs::Arm::Machine>
{
public:
	Emuballs::Arm::Cpu cpu;
	Emuballs::Arm::OpDecoder decoder;
	Emuballs::Memory memory;
	Emuballs::Arm::Prefetch prefetch;

	void flushPrefetch()
	{
		prefetch.flush();
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
	RegisterSet &regs = cpu().regs();
	regval pc = regs.pc();
	bool pcWasChanged = false;
	// Decode opcode.
	OpcodePtr opcode;
	try
	{
		opcode = d->decoder.decode(instruction);
	}
	catch (const OpDecodeError &error)
	{
		std::stringstream ss;
		ss << error.what() << "; instruction addr = 0x" << std::hex << pc - PREFETCH_SIZE;
		throw OpDecodeError(ss.str());
	}
	// Execute opcode.
	regs.resetPcChanged();
	opcode->execute(*this);
	pcWasChanged = regs.wasPcChanged();
	// Flush prefetched instructions if pc register changed due to opcode
	// execution.
	if (pcWasChanged)
	{
		d->flushPrefetch();
	}
}
