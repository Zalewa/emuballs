#include "armmachine.hpp"

using namespace Machine::Arm;

uint32_t Flags::dump() const
{
	return bits.to_ulong();
}

void Flags::store(uint32_t bitset)
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
};

DPointered(Cpu);

Flags &Cpu::flags()
{
	return d->flags;
}

///////////////////////////////////////////////////////////////////////////

DClass<Machine::Arm::Machine>
{
public:
	Cpu cpu;
};

DPointered(Machine::Arm::Machine);

Cpu &Machine::Arm::Machine::cpu()
{
	return d->cpu;
}
