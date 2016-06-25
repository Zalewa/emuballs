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
#define BOOST_TEST_MODULE armopcode_single_data_swap
#include <boost/test/unit_test.hpp>
#include "arm_program_fixture.hpp"
#include "src/emuballs/armopcode_impl.hpp"
#include "src/emuballs/errors.hpp"

static Emuballs::Arm::OpcodePtr decode(uint32_t code)
{
	return Emuballs::Arm::opcodeSingleDataSwap(code);
}

constexpr uint32_t MEM_ADDR = 0x40000;

BOOST_FIXTURE_TEST_SUITE(suite, ArmProgramFixture)

BOOST_AUTO_TEST_CASE(swp)
{
	auto op = decode(0xe1010092); // swp r0, r2, [r1]
	machine.memory().putWord(MEM_ADDR, 0xbcde1234);
	r(0, 0xc1c1c1c1);
	r(1, MEM_ADDR);
	r(2, 0xcafebeba);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xbcde1234);
	BOOST_CHECK_EQUAL(r(1), MEM_ADDR);
	BOOST_CHECK_EQUAL(r(2), 0xcafebeba);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_ADDR), 0xcafebeba);
}

BOOST_AUTO_TEST_CASE(swpb)
{
	auto op = decode(0xe1410092); // swp r0, r2, [r1]
	machine.memory().putWord(MEM_ADDR, 0xbcde1234);
	r(0, 0xc1c1c1c1);
	r(1, MEM_ADDR);
	r(2, 0xcafebeba);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0x34);
	BOOST_CHECK_EQUAL(r(1), MEM_ADDR);
	BOOST_CHECK_EQUAL(r(2), 0xcafebeba);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_ADDR), 0xbcde12ba);
}

BOOST_AUTO_TEST_CASE(r15)
{
	using namespace Emuballs;
	BOOST_CHECK_THROW(decode(0xe10f0092)->validate(), IllegalOpcodeError);
	BOOST_CHECK_THROW(decode(0xe101f092)->validate(), IllegalOpcodeError);
	BOOST_CHECK_THROW(decode(0xe101009f)->validate(), IllegalOpcodeError);
	BOOST_CHECK_NO_THROW(decode(0xe1010092)->validate());
}


BOOST_AUTO_TEST_SUITE_END()
