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

	void setMemoryPtr(Memory *memory)
	{
		this->memory = memory;
	}

	void setCpuPtr(Cpu *cpu)
	{
		this->regs = &cpu->regs();
	}

private:
	ArrayQueue<uint32_t, PREFETCH_INSTRUCTIONS> prefetchedInstructions;
	Memory *memory;
	RegisterSet *regs;
	uint8_t *memptr = nullptr;
	memsize membase = -1;

	void collect(Machine &machine)
	{
		while (prefetchedInstructions.size() < PREFETCH_INSTRUCTIONS)
		{
			regval pc = regs->pc();
			memsize pageOffset = pc - membase;
			if (memptr == nullptr || pageOffset > memory->pageSize())
			{
				membase = pc - (pc % memory->pageSize());
				memptr = memory->ptr(membase);
				pageOffset = pc - membase;
			}
			uint32_t instruction = *reinterpret_cast<uint32_t*>(memptr + pageOffset);
			prefetchedInstructions.push(instruction);
			regs->pc(pc + INSTRUCTION_SIZE);
		}
	}

	uint32_t pop()
	{
		return prefetchedInstructions.pop();
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

	PrivData()
	{
		adjustPointers();
	}

	PrivData(PrivData &&other) noexcept
		: PrivData()
	{
		swap(*this, other);
	}

	PrivData(const PrivData &other)
	{
		this->cpu = other.cpu;
		this->decoder = other.decoder;
		this->prefetch = other.prefetch;
		this->memory = other.memory;
		adjustPointers();
	}

	friend void swap(
		PrivData<Emuballs::Arm::Machine> &a,
		PrivData<Emuballs::Arm::Machine> &b) noexcept
	{
		using std::swap;

		swap(a.cpu, b.cpu);
		swap(a.decoder, b.decoder);
		swap(a.prefetch, b.prefetch);
		swap(a.memory, b.memory);
		a.adjustPointers();
		b.adjustPointers();
	}

	PrivData &operator=(PrivData other)
	{
		swap(*this, other);
		return *this;
	}

private:
	void adjustPointers() noexcept
	{
		prefetch.setCpuPtr(&cpu);
		prefetch.setMemoryPtr(&memory);
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
		d->prefetch.flush();
	}
}
