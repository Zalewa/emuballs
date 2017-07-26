/*
 * Copyright 2017 Zalewa <zalewapl@gmail.com>.
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
#define BOOST_TEST_MODULE machine_armopcode_byte_reverse
#include <boost/test/unit_test.hpp>
#include "src/emuballs/armopcode_impl.hpp"
#include "src/emuballs/armmachine.hpp"
#include "arm_program_fixture.hpp"

using namespace Emuballs::Arm;

static Emuballs::Arm::OpcodePtr decode(uint32_t code)
{
	return Emuballs::Arm::opcodeByteReverse(code);
}


BOOST_FIXTURE_TEST_SUITE(suite, ArmProgramFixture)

BOOST_AUTO_TEST_CASE(byte_reverse)
{
	r(4, 0);
	r(7, 0xa1b2c3d4);
	auto op = decode(0xe6bf4f37); // rev r4, r7
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(4), 0xd4c3b2a1);
	BOOST_CHECK_EQUAL(r(7), 0xa1b2c3d4);
}

BOOST_AUTO_TEST_CASE(byte_reverse_halfword)
{
	r(5, 0);
	r(6, 0xcafebeef);
	auto op = decode(0xe6bf5fb6); // rev16 r5, r6
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(5), 0xfecaefbe);
	BOOST_CHECK_EQUAL(r(6), 0xcafebeef);
}

BOOST_AUTO_TEST_CASE(byte_reverse_halfword_signed)
{
	r(9, 0);
	r(10, 0xcafebeef);
	auto op = decode(0xe6ff9fba); // revsh r9, 10
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(9), 0xffffefbe);
	BOOST_CHECK_EQUAL(r(10), 0xcafebeef);

	r(9, 0);
	r(10, 0x0a00fe7f);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(9), 0x7ffeu);
	BOOST_CHECK_EQUAL(r(10), 0x0a00fe7fu);
}

BOOST_AUTO_TEST_SUITE_END()
