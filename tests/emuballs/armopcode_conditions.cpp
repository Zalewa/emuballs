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
#define BOOST_TEST_MODULE machine_armopcode_conditions
#include <boost/test/unit_test.hpp>
#include "src/emuballs/armopcode.hpp"
#include "src/emuballs/armmachine.hpp"
#include "src/emuballs/errors.hpp"

using namespace Emuballs::Arm;

class FakeCode : public Opcode
{
public:
	bool wasRun;

	FakeCode(uint32_t code) : Opcode(code << 28)
	{
		wasRun = false;
	}

	void run(Emuballs::Arm::Machine &machine)
	{
		wasRun = true;
	}
};

static bool execCode(uint32_t conditional, uint32_t flags)
{
	Emuballs::Arm::Machine machine;
	machine.cpu().flags().store(flags);
	FakeCode opcode(conditional);
	opcode.execute(machine);
	return opcode.wasRun;
}

constexpr static auto allflags = 0xf0000000UL;
constexpr static auto noflags = 0UL;

BOOST_AUTO_TEST_CASE(eq_equal)
{
	constexpr auto code = 0b0000;
	BOOST_CHECK(execCode(code, Flags::F_ZERO));
	BOOST_CHECK(execCode(code, allflags));
	BOOST_CHECK(!execCode(code, allflags & ~Flags::F_ZERO));
}

BOOST_AUTO_TEST_CASE(ne_not_equal)
{
	constexpr auto code = 0b0001;
	BOOST_CHECK(!execCode(code, Flags::F_ZERO));
	BOOST_CHECK(!execCode(code, allflags));
	BOOST_CHECK(execCode(code, allflags & ~Flags::F_ZERO));
}

BOOST_AUTO_TEST_CASE(cs_unsigned_higer_or_same)
{
	constexpr auto code = 0b0010;
	BOOST_CHECK(execCode(code, Flags::F_CARRY));
	BOOST_CHECK(execCode(code, allflags));
	BOOST_CHECK(!execCode(code, allflags & ~Flags::F_CARRY));
}

BOOST_AUTO_TEST_CASE(cc_unsigned_lower)
{
	constexpr auto code = 0b0011;
	BOOST_CHECK(!execCode(code, Flags::F_CARRY));
	BOOST_CHECK(!execCode(code, allflags));
	BOOST_CHECK(execCode(code, allflags & ~Flags::F_CARRY));
}

BOOST_AUTO_TEST_CASE(mi_negative)
{
	constexpr auto code = 0b0100;
	BOOST_CHECK(execCode(code, Flags::F_NEGATIVE));
	BOOST_CHECK(execCode(code, allflags));
	BOOST_CHECK(!execCode(code, allflags & ~Flags::F_NEGATIVE));
}

BOOST_AUTO_TEST_CASE(pl_positive_or_zero)
{
	constexpr auto code = 0b0101;
	BOOST_CHECK(!execCode(code, Flags::F_NEGATIVE));
	BOOST_CHECK(!execCode(code, allflags));
	BOOST_CHECK(execCode(code, allflags & ~Flags::F_NEGATIVE));
}

BOOST_AUTO_TEST_CASE(vs_overflow)
{
	constexpr auto code = 0b0110;
	BOOST_CHECK(execCode(code, Flags::F_OVERFLOW));
	BOOST_CHECK(execCode(code, allflags));
	BOOST_CHECK(!execCode(code, allflags & ~Flags::F_OVERFLOW));
}

BOOST_AUTO_TEST_CASE(vc_no_overflow)
{
	constexpr auto code = 0b0111;
	BOOST_CHECK(!execCode(code, Flags::F_OVERFLOW));
	BOOST_CHECK(!execCode(code, allflags));
	BOOST_CHECK(execCode(code, allflags & ~Flags::F_OVERFLOW));
}

BOOST_AUTO_TEST_CASE(hi_unsigned_higher)
{
	constexpr auto code = 0b1000;
	BOOST_CHECK(execCode(code, Flags::F_CARRY));
	BOOST_CHECK(execCode(code, allflags & ~Flags::F_ZERO));
	BOOST_CHECK(!execCode(code, allflags));
	BOOST_CHECK(!execCode(code, allflags & ~(Flags::F_ZERO | Flags::F_CARRY)));
}

BOOST_AUTO_TEST_CASE(ls_unsigned_lower_or_same)
{
	constexpr auto code = 0b1001;
	BOOST_CHECK(execCode(code, allflags & ~Flags::F_CARRY));
	BOOST_CHECK(execCode(code, Flags::F_ZERO));
	BOOST_CHECK(execCode(code, allflags & ~(Flags::F_ZERO | Flags::F_CARRY)));
	BOOST_CHECK(execCode(code, noflags));
	BOOST_CHECK(!execCode(code, Flags::F_CARRY));
}

BOOST_AUTO_TEST_CASE(ge_greater_or_equal)
{
	constexpr auto code = 0b1010;
	BOOST_CHECK(execCode(code, noflags));
	BOOST_CHECK(execCode(code, allflags));
	BOOST_CHECK(execCode(code, Flags::F_NEGATIVE | Flags::F_OVERFLOW));
	BOOST_CHECK(execCode(code, allflags & ~(Flags::F_NEGATIVE | Flags::F_OVERFLOW)));
	BOOST_CHECK(!execCode(code, Flags::F_NEGATIVE));
	BOOST_CHECK(!execCode(code, Flags::F_OVERFLOW));
	BOOST_CHECK(!execCode(code, allflags & ~(Flags::F_NEGATIVE)));
	BOOST_CHECK(!execCode(code, allflags & ~(Flags::F_OVERFLOW)));
}

BOOST_AUTO_TEST_CASE(lt_less_than)
{
	constexpr auto code = 0b1011;
	BOOST_CHECK(!execCode(code, noflags));
	BOOST_CHECK(!execCode(code, allflags));
	BOOST_CHECK(!execCode(code, Flags::F_NEGATIVE | Flags::F_OVERFLOW));
	BOOST_CHECK(!execCode(code, allflags & ~(Flags::F_NEGATIVE | Flags::F_OVERFLOW)));
	BOOST_CHECK(execCode(code, Flags::F_NEGATIVE));
	BOOST_CHECK(execCode(code, Flags::F_OVERFLOW));
	BOOST_CHECK(execCode(code, allflags & ~(Flags::F_NEGATIVE)));
	BOOST_CHECK(execCode(code, allflags & ~(Flags::F_OVERFLOW)));
}

BOOST_AUTO_TEST_CASE(gt_greater_than)
{
	constexpr auto code = 0b1100;
	BOOST_CHECK(execCode(code, noflags));
	BOOST_CHECK(execCode(code, allflags & ~Flags::F_ZERO));
	BOOST_CHECK(execCode(code, Flags::F_NEGATIVE | Flags::F_OVERFLOW));
	BOOST_CHECK(execCode(code, allflags & ~(Flags::F_ZERO | Flags::F_NEGATIVE | Flags::F_OVERFLOW)));
	BOOST_CHECK(!execCode(code, Flags::F_ZERO));
	BOOST_CHECK(!execCode(code, Flags::F_NEGATIVE));
	BOOST_CHECK(!execCode(code, Flags::F_OVERFLOW));
	BOOST_CHECK(!execCode(code, Flags::F_ZERO | Flags::F_NEGATIVE | Flags::F_OVERFLOW));
	BOOST_CHECK(!execCode(code, allflags & ~(Flags::F_ZERO | Flags::F_NEGATIVE)));
	BOOST_CHECK(!execCode(code, allflags & ~(Flags::F_ZERO | Flags::F_OVERFLOW)));
}

BOOST_AUTO_TEST_CASE(le_less_than_or_equal)
{
	constexpr auto code = 0b1101;
	BOOST_CHECK(!execCode(code, noflags));
	BOOST_CHECK(!execCode(code, allflags & ~Flags::F_ZERO));
	BOOST_CHECK(execCode(code, allflags));
	BOOST_CHECK(!execCode(code, Flags::F_NEGATIVE | Flags::F_OVERFLOW));
	BOOST_CHECK(!execCode(code, allflags & ~(Flags::F_ZERO | Flags::F_NEGATIVE
		| Flags::F_OVERFLOW)));
	BOOST_CHECK(execCode(code, Flags::F_ZERO));
	BOOST_CHECK(execCode(code, Flags::F_NEGATIVE));
	BOOST_CHECK(execCode(code, Flags::F_OVERFLOW));
	BOOST_CHECK(execCode(code, Flags::F_ZERO | Flags::F_NEGATIVE | Flags::F_OVERFLOW));
	BOOST_CHECK(execCode(code, allflags & ~(Flags::F_ZERO | Flags::F_NEGATIVE)));
	BOOST_CHECK(execCode(code, allflags & ~(Flags::F_ZERO | Flags::F_OVERFLOW)));
}

BOOST_AUTO_TEST_CASE(al_always)
{
	constexpr auto code = 0b1110;
	for (uint32_t i = 0; i < 16; ++i)
	{
		BOOST_CHECK_MESSAGE(execCode(code, i << 28), std::bitset<4>(i).to_string());
	}
}

BOOST_AUTO_TEST_CASE(illegal)
{
	constexpr auto code = 0b1111;
	BOOST_CHECK_THROW(execCode(code, noflags), Emuballs::IllegalOpcodeError);
}
