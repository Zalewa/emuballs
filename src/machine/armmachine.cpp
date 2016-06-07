#include "armmachine.hpp"
#include "memory.hpp"
#include "opdecoder.hpp"
#include "errors.hpp"

#include <queue>
#include <sstream>

namespace Machine { namespace Arm {

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

namespace Machine { namespace Arm {

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

}}

///////////////////////////////////////////////////////////////////////////

DClass<Machine::Arm::Cpu>
{
public:
	Machine::Arm::Flags flags;
	Machine::Arm::RegisterSet regs;
};

DPointered(Machine::Arm::Cpu);

namespace Machine { namespace Arm {

const Flags &Cpu::flags() const
{
	return d->flags;
}

Flags &Cpu::flags()
{
	return d->flags;
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

namespace Machine { namespace Arm
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
}}

DClass<Machine::Arm::Machine>
{
public:
	Machine::Arm::Cpu cpu;
	Machine::Memory memory;
	Machine::Arm::Prefetch prefetch;

	void flushPrefetch()
	{
		prefetch = Machine::Arm::Prefetch();
	}
};

DPointered(Machine::Arm::Machine);

Machine::Arm::Cpu &Machine::Arm::Machine::cpu()
{
	return d->cpu;
}

Machine::Memory &Machine::Arm::Machine::memory()
{
	return d->memory;
}

void Machine::Arm::Machine::cycle()
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
		ss << error.what() << "; instruction addr = " << pc - PREFETCH_SIZE;
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
