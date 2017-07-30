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
#include "armopcode.hpp"
#include "armmachine.hpp"

#include "errors_private.hpp"

namespace Emuballs
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

Opcode::Opcode(uint32_t code)
{
	this->m_code = code;
}

void Opcode::execute(Machine &machine)
{
	auto condition = (m_code >> 28) & 0xf;
	if (Conditional::met(condition, machine.cpu().flags()))
	{
		run(machine);
	}
}

void Opcode::validate()
{
}

} // namespace Arm

} // namespace Emuballs
