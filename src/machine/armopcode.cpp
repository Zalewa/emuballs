#include "armopcode.hpp"
#include "armmachine.hpp"

namespace Machine
{

class ArmConditional
{
public:
	enum Cond
	{
		eq = 0,
		ne,
		cs,
		cc,
		mi,
		pl,
		vs,
		vc,
		hi,
		ls,
		ge,
		lt,
		gt,
		le,
		al
	};

	static bool met(uint8_t condition, const ArmFlags &flags)
	{
		switch (condition)
		{
		case al:
			return true;
		case eq:
			return flags.test(ArmFlags::Zero);
		case ne:
			return !flags.test(ArmFlags::Zero);
		case cs:
			return flags.test(ArmFlags::Carry);
		case cc:
			return !flags.test(ArmFlags::Carry);
		case mi:
			return flags.test(ArmFlags::Negative);
		case pl:
			return !flags.test(ArmFlags::Negative);
		case vs:
			return flags.test(ArmFlags::Overflow);
		case vc:
			return !flags.test(ArmFlags::Overflow);
		case hi:
			return flags.test(ArmFlags::Carry) && !flags.test(ArmFlags::Zero);
		case ls:
			return !flags.test(ArmFlags::Carry) || flags.test(ArmFlags::Zero);
		case ge:
			return flags.test(ArmFlags::Negative) == flags.test(ArmFlags::Overflow);
		case lt:
			return flags.test(ArmFlags::Negative) != flags.test(ArmFlags::Overflow);
		case gt:
			return !flags.test(ArmFlags::Zero)
				&& (flags.test(ArmFlags::Negative) == flags.test(ArmFlags::Overflow));
		case le:
			return flags.test(ArmFlags::Zero)
				|| (flags.test(ArmFlags::Negative) != flags.test(ArmFlags::Overflow));
		default:
			throw IllegalOpcodeError("unknown condition");
		}
	}
};

}

using namespace Machine;

ArmOpcode::ArmOpcode(uint32_t code)
{
	this->code = code;
}

void ArmOpcode::execute(ArmMachine &machine)
{
	auto condition = (code >> 28) & 0xf;
	if (ArmConditional::met(condition, machine.cpu().flags()))
	{
		run();
	}
}
