#include "armopcode.hpp"

using namespace Machine;

ArmOpCode::ArmOpCode(uint32_t code)
{
	this->code = code;
}

void ArmOpCode::execute(Machine &machine)
{
	// TODO: Understand conditional flags.
}
