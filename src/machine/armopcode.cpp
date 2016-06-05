#include "armopcode.hpp"
#include "armmachine.hpp"

namespace Machine
{

namespace Arm
{

class Conditional
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

	static bool met(uint8_t condition, const Flags &flags)
	{
		switch (condition)
		{
		case al:
			return true;
		case eq:
			return flags.test(Flags::Zero);
		case ne:
			return !flags.test(Flags::Zero);
		case cs:
			return flags.test(Flags::Carry);
		case cc:
			return !flags.test(Flags::Carry);
		case mi:
			return flags.test(Flags::Negative);
		case pl:
			return !flags.test(Flags::Negative);
		case vs:
			return flags.test(Flags::Overflow);
		case vc:
			return !flags.test(Flags::Overflow);
		case hi:
			return flags.test(Flags::Carry) && !flags.test(Flags::Zero);
		case ls:
			return !flags.test(Flags::Carry) || flags.test(Flags::Zero);
		case ge:
			return flags.test(Flags::Negative) == flags.test(Flags::Overflow);
		case lt:
			return flags.test(Flags::Negative) != flags.test(Flags::Overflow);
		case gt:
			return !flags.test(Flags::Zero)
				&& (flags.test(Flags::Negative) == flags.test(Flags::Overflow));
		case le:
			return flags.test(Flags::Zero)
				|| (flags.test(Flags::Negative) != flags.test(Flags::Overflow));
		default:
			throw IllegalOpcodeError("unknown condition");
		}
	}
};

} // namespace Arm

} // namespace Machine

using namespace Machine::Arm;

Opcode::Opcode(uint32_t code)
{
	this->m_code = code;
}

uint32_t Opcode::code() const
{
	return m_code;
}

void Opcode::execute(Machine &machine)
{
	auto condition = (m_code >> 28) & 0xf;
	if (Conditional::met(condition, machine.cpu().flags()))
	{
		run();
	}
}
