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
#include "opdecoder.hpp"

#include <queue>
#include <sstream>

namespace Emuballs { namespace Arm
{
class Prefetch
{
public:
	uint32_t next()
	{
		collect();
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

	void collect()
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
	Emuballs::Arm::OpDecoder decoder;
	Emuballs::Arm::Prefetch prefetch;
};

DPointered(Emuballs::Arm::Machine);

}

Emuballs::Arm::Machine::Machine()
{
	adjustPointers();
}

Emuballs::Arm::Machine::Machine(const Machine &other)
{
	this->_cpu = other._cpu;
	this->_memory = other._memory;
	this->d = other.d;
	adjustPointers();
}

Emuballs::Arm::Machine::Machine(Machine && other) noexcept
{
	swap(*this, other);
}

Emuballs::Arm::Machine &Emuballs::Arm::Machine::operator=(Machine other)
{
	swap(*this, other);
	return *this;
}

void Emuballs::Arm::Machine::adjustPointers() noexcept
{
	d->prefetch.setCpuPtr(&_cpu);
	d->prefetch.setMemoryPtr(&_memory);
}

void Emuballs::Arm::Machine::cycle()
{
	// Prefetch instructions and get next instruction to execute.
	auto instruction = d->prefetch.next();
	RegisterSet &regs = cpu().regs();
	regval pc = regs.pc();
	bool pcWasChanged = false;
	// Decode opcode.
	Opcode* opcode;
	try
	{
		opcode = d->decoder.decode(pc - PREFETCH_SIZE, instruction);
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
