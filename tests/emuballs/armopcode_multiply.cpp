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
#define BOOST_TEST_MODULE armopcode_multiply
#include <boost/test/unit_test.hpp>
#include "arm_program_fixture.hpp"
#include "src/emuballs/armopcode_impl.hpp"
#include "src/emuballs/errors_private.hpp"

static Emuballs::Arm::OpcodePtr decode(uint32_t code)
{
	auto op = Emuballs::Arm::opcodeMultiply(code);
	op->validate();
	return op;
}

BOOST_FIXTURE_TEST_SUITE(suite, ArmProgramFixture)

// cond 0000 00as  rd  rn  rs 1001 rm
//
// mul rd, rm, rs
// mla rd, rm, rs, rn

BOOST_AUTO_TEST_CASE(muls)
{
	auto op = decode(0xe0100291); // muls r0, r1, r2

	flags().zero(false);
	flags().negative(false);
	r(0, 0);
	r(1, 2);
	r(2, 3);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 6);
	BOOST_CHECK_EQUAL(r(1), 2);
	BOOST_CHECK_EQUAL(r(2), 3);
	BOOST_CHECK(!flags().zero());
	BOOST_CHECK(!flags().negative());

	flags().zero(false);
	flags().negative(false);
	r(0, 0);
	r(1, 0x7fffffff);
	r(2, 0xffffffff);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0x80000001);
	BOOST_CHECK(!flags().zero());
	BOOST_CHECK(flags().negative());

	flags().zero(false);
	flags().negative(false);
	r(0, 0xff);
	r(1, 0);
	r(2, 15);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0);
	BOOST_CHECK(flags().zero());
	BOOST_CHECK(!flags().negative());

	flags().zero(false);
	flags().negative(false);
	r(0, 0xff);
	r(1, 0xffffffff);
	r(2, 0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0);
	BOOST_CHECK(flags().zero());
	BOOST_CHECK(!flags().negative());
}

BOOST_AUTO_TEST_CASE(mul_rd_rs_same)
{
	auto op = decode(0xe0000091); // mul r0, r1, r0

	flags().zero(true);
	flags().negative(true);
	r(0, 3);
	r(1, 2);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 6);
	BOOST_CHECK_EQUAL(r(1), 2);
	BOOST_CHECK(flags().zero());
	BOOST_CHECK(flags().negative());

	flags().zero(true);
	flags().negative(false);
	r(0, 0xffffffff);
	r(1, 0x7fffffff);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0x80000001);
	BOOST_CHECK(flags().zero());
	BOOST_CHECK(!flags().negative());

	flags().zero(false);
	flags().negative(true);
	r(0, 15);
	r(1, 0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0);
	BOOST_CHECK(!flags().zero());
	BOOST_CHECK(flags().negative());

	flags().zero(true);
	flags().negative(false);
	r(0, 0);
	r(1, 0xffffffff);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0);
	BOOST_CHECK(flags().zero());
	BOOST_CHECK(!flags().negative());
}

BOOST_AUTO_TEST_CASE(mla_rd_rs_same_rm_rn_same)
{
	auto op = decode(0xe0234394); // mla r3, r4, r3, r4

	r(3, 3);
	r(4, 2);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(3), 8);
	BOOST_CHECK_EQUAL(r(4), 2);

	r(3, 0xffffffff);
	r(4, 0x7fffffff);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0);

	r(3, 15);
	r(4, 0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0);

	r(3, 0);
	r(4, 0xffffffff);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0);
}

BOOST_AUTO_TEST_CASE(mlas_rd_rn_same)
{
	auto op = decode(0xe0333294); // mlas r3, r4, r2, r3

	flags().zero(false);
	flags().negative(false);
	r(3, 14);
	r(2, 2);
	r(4, 3);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(3), 20);
	BOOST_CHECK_EQUAL(r(2), 2);
	BOOST_CHECK_EQUAL(r(4), 3);
	BOOST_CHECK(!flags().zero());
	BOOST_CHECK(!flags().negative());

	flags().zero(false);
	flags().negative(false);
	r(3, 1);
	r(2, 0x7fffffff);
	r(4, 0xffffffff);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(3), 0x80000002);
	BOOST_CHECK(!flags().zero());
	BOOST_CHECK(flags().negative());

	flags().zero(false);
	flags().negative(false);
	r(3, 0xff);
	r(2, 0);
	r(4, 15);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(3), 0xff);
	BOOST_CHECK(!flags().zero());
	BOOST_CHECK(!flags().negative());

	flags().zero(false);
	flags().negative(false);
	r(3, 0xffffffff);
	r(2, 1);
	r(4, 1);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(3), 0);
	BOOST_CHECK(flags().zero());
	BOOST_CHECK(!flags().negative());
}

BOOST_AUTO_TEST_CASE(r15)
{
	using namespace Emuballs;
	BOOST_CHECK_THROW(decode(0xe023f394)->validate(), IllegalOpcodeError); // mla r3, r4, r3, r15
	BOOST_CHECK_THROW(decode(0xe0234f94)->validate(), IllegalOpcodeError); // mla r3, r4, r15, r4
	BOOST_CHECK_THROW(decode(0xe023439f)->validate(), IllegalOpcodeError); // mla r3, r15, r3, r4
	BOOST_CHECK_NO_THROW(decode(0xe0234394)->validate()); // mla r3, r4, r3, r4
}

BOOST_AUTO_TEST_CASE(same_regs)
{
	using namespace Emuballs;
	BOOST_CHECK_THROW(decode(0xe0235493)->validate(), IllegalOpcodeError); // mla r3, r3, r4, r5
	BOOST_CHECK_NO_THROW(decode(0xe0234394)->validate()); // mla r3, r4, r3, r4
}

BOOST_AUTO_TEST_SUITE_END()
