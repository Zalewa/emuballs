#include "armmachine.hpp"
#include "memory.hpp"

using namespace Machine;
using namespace Machine::Arm;

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

///////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////

DClass<Cpu>
{
public:
	Flags flags;
	RegisterSet regs;
};

DPointered(Cpu);

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

///////////////////////////////////////////////////////////////////////////

DClass<Machine::Arm::Machine>
{
public:
	Cpu cpu;
	Memory memory;
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
	cpu().regs().pc() += INSTRUCTION_SIZE;
}
