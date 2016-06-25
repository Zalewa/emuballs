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
#define BOOST_TEST_MODULE machine_programs
#include <boost/test/unit_test.hpp>
#include <iterator>
#include "src/emuballs/armmachine.hpp"
#include "src/emuballs/memory.hpp"
#include "arm_program_fixture.hpp"

using namespace Emuballs::Arm;

BOOST_FIXTURE_TEST_SUITE(suite, ArmProgramFixture);

BOOST_AUTO_TEST_CASE(regclone)
{
	constexpr uint32_t code[] = {
		// 00000000 <regclone>:
		0xe1a01000, // mov	r1, r0
		0xe1a02000, // mov	r2, r0
		0xe1a03000, // mov	r3, r0
		0xe1a04000, // mov	r4, r0
		0xe1a05000, // mov	r5, r0
		0xe1a06000, // mov	r6, r0
		0xe1a07000, // mov	r7, r0
		0xe1a08000, // mov	r8, r0
		0xe1a09000, // mov	r9, r0
		0xe1a0a000, // mov	sl, r0
		0xe1a0b000, // mov	fp, r0
		0xe1a0c000, // mov	ip, r0
		0xe1a0f00e, // mov	pc, lr
	};

	constexpr auto AFFECTED_REGS = 13;
	constexpr auto PARAM = 0xa1b2c3d4;
	load(std::begin(code), std::end(code));
	for (auto i = 0; i < AFFECTED_REGS; ++i)
	{
		r(i, 1234);
	}
	r(0, PARAM);
	runProgram();
	BOOST_CHECK_EQUAL(r(0), PARAM);
	BOOST_CHECK_EQUAL(r(1), PARAM);
	BOOST_CHECK_EQUAL(r(0), PARAM);
	BOOST_CHECK_EQUAL(r(2), PARAM);
	BOOST_CHECK_EQUAL(r(0), PARAM);
	BOOST_CHECK_EQUAL(r(3), PARAM);
	BOOST_CHECK_EQUAL(r(4), PARAM);
	BOOST_CHECK_EQUAL(r(5), PARAM);
	BOOST_CHECK_EQUAL(r(6), PARAM);
	BOOST_CHECK_EQUAL(r(7), PARAM);
	BOOST_CHECK_EQUAL(r(8), PARAM);
	BOOST_CHECK_EQUAL(r(9), PARAM);
	BOOST_CHECK_EQUAL(r(10), PARAM);
	BOOST_CHECK_EQUAL(r(11), PARAM);
	BOOST_CHECK_EQUAL(r(12), PARAM);
}

BOOST_AUTO_TEST_CASE(fibonacci)
{
	constexpr uint32_t code[] = {
		// 00000000 <fibonacci>:
		0xe1a03000, // mov	r3, r0
		0xe3330000, // teq	r3, #0
		0x03a00000, // moveq	r0, #0
		0x01a0f00e, // moveq	pc, lr
		0xe3330001, // teq	r3, #1
		0x03a00001, // moveq	r0, #1
		0x01a0f00e, // moveq	pc, lr
		0xe3a01000, // mov	r1, #0
		0xe3a02001, // mov	r2, #1
		// 00000024 <_fib_loop>:
		0xe3330001, // teq	r3, #1
		0x01a0f00e, // moveq	pc, lr
		0xe0810002, // add	r0, r1, r2
		0xe1a01002, // mov	r1, r2
		0xe1a02000, // mov	r2, r0
		0xe2433001, // sub	r3, r3, #1
		0xeafffff8, // b	24 <_fib_loop>
	};
	load(std::begin(code), std::end(code));
	r(0, 10);
	runProgram();
	BOOST_CHECK_EQUAL(r(0), 55);

	reset();
	r(0, 18);
	runProgram();
	BOOST_CHECK_EQUAL(r(0), 2584);

	reset();
	r(0, 0);
	runProgram();
	BOOST_CHECK_EQUAL(r(0), 0);

	reset();
	r(0, 1);
	runProgram();
	BOOST_CHECK_EQUAL(r(0), 1);

	reset();
	r(0, 2);
	runProgram();
	BOOST_CHECK_EQUAL(r(0), 1);

	reset();
	r(0, 3);
	runProgram();
	BOOST_CHECK_EQUAL(r(0), 2);
}

BOOST_AUTO_TEST_CASE(mul)
{
	constexpr uint32_t code[] = {
		// 00000000 <mul>:
		0xe0000291, // mul	r0, r1, r2
		0xe1a0f00e, // mov	pc, lr
	};

	load(std::begin(code), std::end(code));
	r(0, 0);
	r(1, 4);
	r(2, 5);
	runProgram();
	BOOST_CHECK_EQUAL(r(0), 20);
}

BOOST_AUTO_TEST_CASE(lsl)
{
	constexpr uint32_t code[] = {
		// 00000000 <BitShift>:
		0xe1a02000, // mov	r2, r0
		0xe1a00112, // lsl	r0, r2, r1
		0xe1a0f00e, // mov	pc, lr
	};

	load(std::begin(code), std::end(code));
	r(0, 0b11);
	r(1, 4);
	runProgram();
	BOOST_CHECK_EQUAL(r(0), 0b110000);
	BOOST_CHECK(!flags().carry());

	for (int initCarry = 0; initCarry <= 1; ++initCarry)
	{
		reset();
		bool carry = initCarry != 0;
		flags().carry(carry);
		r(0, 0xf0000000);
		r(1, 1);
		runProgram();
		BOOST_CHECK_EQUAL(r(0), 0xe0000000);
		BOOST_CHECK_EQUAL(flags().carry(), carry);
	}
}

BOOST_AUTO_TEST_CASE(lsr)
{
	constexpr uint32_t code[] = {
		// 00000000 <BitShift>:
		0xe1a02000, // mov	r2, r0
		0xe1a00132, // lsr	r0, r2, r1
		0xe1a0f00e, // mov	pc, lr
	};

	load(std::begin(code), std::end(code));
	r(0, 0b110000);
	r(1, 4);
	runProgram();
	BOOST_CHECK_EQUAL(r(0), 0b11);
	BOOST_CHECK(!flags().carry());

	for (int initCarry = 0; initCarry <= 1; ++initCarry)
	{
		reset();
		bool carry = initCarry != 0;
		flags().carry(carry);
		r(0, 0b1111);
		r(1, 1);
		runProgram();
		BOOST_CHECK_EQUAL(r(0), 0b111);
		BOOST_CHECK_EQUAL(flags().carry(), carry);
	}
}

BOOST_AUTO_TEST_CASE(asr)
{
	constexpr uint32_t code[] = {
		// 00000000 <BitShift>:
		0xe1a02000, // mov	r2, r0
		0xe1a00152, // asr	r0, r2, r1
		0xe1a0f00e, // mov	pc, lr
	};

	load(std::begin(code), std::end(code));
	r(0, 0x80008000);
	r(1, 8);
	runProgram();
	BOOST_CHECK_EQUAL(r(0), 0xff800080);
	BOOST_CHECK(!flags().carry());

	for (int initCarry = 0; initCarry <= 1; ++initCarry)
	{
		reset();
		bool carry = initCarry != 0;
		flags().carry(carry);
		r(0, 0xff);
		r(1, 3);
		runProgram();
		BOOST_CHECK_EQUAL(r(0), 0x1f);
		BOOST_CHECK_EQUAL(flags().carry(), carry);
	}
}

BOOST_AUTO_TEST_CASE(ror)
{
	constexpr uint32_t code[] = {
		// 00000000 <BitShift>:
		0xe1a02000, // mov	r2, r0
		0xe1a00172, // ror	r0, r2, r1
		0xe1a0f00e, // mov	pc, lr
	};

	load(std::begin(code), std::end(code));
	r(0, 0b11);
	r(1, 4);
	runProgram();
	BOOST_CHECK_EQUAL(r(0), 0x30000000);
	BOOST_CHECK(!flags().carry());

	for (int initCarry = 0; initCarry <= 1; ++initCarry)
	{
		reset();
		bool carry = initCarry != 0;
		flags().carry(carry);
		r(0, 0xff);
		r(1, 3);
		runProgram();
		BOOST_CHECK_EQUAL(r(0), 0xe000001f);
		BOOST_CHECK_EQUAL(flags().carry(), carry);
	}
}

BOOST_AUTO_TEST_CASE(lsls)
{
	constexpr uint32_t code[] = {
		// 00000000 <BitShift>:
		0xe1a02000, // mov	r2, r0
		0xe1b00112, // lsls	r0, r2, r1
		0xe1a0f00e, // mov	pc, lr
	};

	load(std::begin(code), std::end(code));
	r(0, 0b11);
	r(1, 4);
	runProgram();
	BOOST_CHECK_EQUAL(r(0), 0b110000);
	BOOST_CHECK(!flags().carry());

	for (int initCarry = 0; initCarry <= 1; ++initCarry)
	{
		reset();
		bool carry = initCarry != 0;
		flags().carry(carry);
		r(0, 0xf0000000);
		r(1, 1);
		runProgram();
		BOOST_CHECK_EQUAL(r(0), 0xe0000000);
		BOOST_CHECK(flags().carry());
	}
}

BOOST_AUTO_TEST_CASE(lsrs)
{
	constexpr uint32_t code[] = {
		// 00000000 <BitShift>:
		0xe1a02000, // mov	r2, r0
		0xe1b00132, // lsrs	r0, r2, r1
		0xe1a0f00e, // mov	pc, lr
	};

	load(std::begin(code), std::end(code));
	r(0, 0b110000);
	r(1, 4);
	runProgram();
	BOOST_CHECK_EQUAL(r(0), 0b11);
	BOOST_CHECK(!flags().carry());

	for (int initCarry = 0; initCarry <= 1; ++initCarry)
	{
		reset();
		bool carry = initCarry != 0;
		flags().carry(carry);
		r(0, 0b1111);
		r(1, 1);
		runProgram();
		BOOST_CHECK_EQUAL(r(0), 0b111);
		BOOST_CHECK(flags().carry());
	}
}

BOOST_AUTO_TEST_CASE(asrs)
{
	constexpr uint32_t code[] = {
		// 00000000 <BitShift>:
		0xe1a02000, // mov	r2, r0
		0xe1b00152, // asrs	r0, r2, r1
		0xe1a0f00e, // mov	pc, lr
	};

	load(std::begin(code), std::end(code));
	r(0, 0x80008000);
	r(1, 8);
	runProgram();
	BOOST_CHECK_EQUAL(r(0), 0xff800080);
	BOOST_CHECK(!flags().carry());

	for (int initCarry = 0; initCarry <= 1; ++initCarry)
	{
		reset();
		bool carry = initCarry != 0;
		flags().carry(carry);
		r(0, 0xff);
		r(1, 3);
		runProgram();
		BOOST_CHECK_EQUAL(r(0), 0x1f);
		BOOST_CHECK(flags().carry());
	}
}

BOOST_AUTO_TEST_CASE(rors)
{
	constexpr uint32_t code[] = {
		// 00000000 <BitShift>:
		0xe1a02000, // mov	r2, r0
		0xe1b00172, // rors	r0, r2, r1
		0xe1a0f00e, // mov	pc, lr
	};

	load(std::begin(code), std::end(code));
	r(0, 0x3);
	r(1, 8);
	runProgram();
	BOOST_CHECK_EQUAL(r(0), 0x03000000);
	BOOST_CHECK(!flags().carry());

	for (int initCarry = 0; initCarry <= 1; ++initCarry)
	{
		reset();
		bool carry = initCarry != 0;
		flags().carry(carry);
		r(0, 0xff);
		r(1, 3);
		runProgram();
		BOOST_CHECK_EQUAL(r(0), 0xe000001f);
		BOOST_CHECK(flags().carry());
	}
}

BOOST_AUTO_TEST_CASE(rrx)
{
	constexpr uint32_t code[] = {
		// 00000028 <RotateRightExtended>:
		0xe1a00060, // rrx	r0, r0
		0xe1a0f00e, // mov	pc, lr
	};

	load(std::begin(code), std::end(code));
	r(0, 0x3);
	flags().carry(false);
	runProgram();
	BOOST_CHECK_EQUAL(r(0), 0x1);
	BOOST_CHECK(!flags().carry());

	reset();
	r(0, 0x40);
	flags().carry(true);
	runProgram();
	BOOST_CHECK_EQUAL(r(0), 0x80000020);
	BOOST_CHECK(flags().carry());
}

BOOST_AUTO_TEST_CASE(rrxs)
{
	constexpr uint32_t code[] = {
		// 00000028 <RotateRightExtended>:
		0xe1b00060, // rrxs	r0, r0
		0xe1a0f00e, // mov	pc, lr
	};

	load(std::begin(code), std::end(code));
	r(0, 0x3);
	flags().carry(false);
	runProgram();
	BOOST_CHECK_EQUAL(r(0), 0x1);
	BOOST_CHECK(flags().carry());

	reset();
	r(0, 0x40);
	flags().carry(true);
	runProgram();
	BOOST_CHECK_EQUAL(r(0), 0x80000020);
	BOOST_CHECK(!flags().carry());
}

BOOST_AUTO_TEST_SUITE_END()
