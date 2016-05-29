#include "armmachine.hpp"

using namespace Machine;

uint32_t ArmFlags::dump() const
{
	return bits.to_ulong();
}

void ArmFlags::set(Bit bit, bool state)
{
	bits.set(bit, state);
}

bool ArmFlags::test(Bit bit) const
{
	return bits.test(bit);
}

///////////////////////////////////////////////////////////////////////////

DClass<ArmCpu>
{
public:
	ArmFlags flags;
};

DPointered(ArmCpu);

ArmFlags &ArmCpu::flags()
{
	return d->flags;
}

///////////////////////////////////////////////////////////////////////////

DClass<ArmMachine>
{
public:
	ArmCpu cpu;
};

DPointered(ArmMachine);

ArmCpu &ArmMachine::cpu()
{
	return d->cpu;
}
