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
#define BOOST_TEST_MODULE shifts
#include <boost/test/unit_test.hpp>
#include "src/emuballs/shift.hpp"

#include <cstdint>
#include <functional>

BOOST_AUTO_TEST_CASE(logicalLeft)
{
	auto f = Emuballs::Arm::logicalLeft<uint32_t>;
	bool carry = false;
	BOOST_CHECK_EQUAL(0x8, f(1, 3, &carry));
	BOOST_CHECK(!carry);
	BOOST_CHECK_EQUAL(0b1011011, f(0b1011011, 0, &carry));
	BOOST_CHECK(!carry);
	carry = true;
	f(0b1011011, 0, &carry);
	BOOST_CHECK(carry);
	BOOST_CHECK_EQUAL(0b101101100, f(0b1011011, 2, &carry));
	BOOST_CHECK(!carry);
	BOOST_CHECK_EQUAL(0, f(0b1011011, 32, &carry));
	BOOST_CHECK(carry);
	BOOST_CHECK_EQUAL(0x80000000, f(0b1011011, 31, &carry));
	BOOST_CHECK(carry);
	BOOST_CHECK_EQUAL(0b10110000, f(0b1011, 4, &carry));
	BOOST_CHECK(!carry);
	BOOST_CHECK_EQUAL(0b10110, f(0b1011, 1, &carry));
	BOOST_CHECK(!carry);
	BOOST_CHECK_EQUAL(0xffffffff, f(0xffffffff, 0, &carry));
	BOOST_CHECK(!carry);
	BOOST_CHECK_EQUAL(0xfffffffe, f(0xffffffff, 1, &carry));
	BOOST_CHECK(carry);
}

BOOST_AUTO_TEST_CASE(logicalRight)
{
	auto f = Emuballs::Arm::logicalRight<uint32_t>;
	bool carry = false;
	BOOST_CHECK_EQUAL(0x1, f(0x8, 3, &carry));
	BOOST_CHECK(!carry);
	BOOST_CHECK_EQUAL(0b1011011, f(0b1011011, 0, &carry));
	BOOST_CHECK(!carry);
	carry = true;
	BOOST_CHECK_EQUAL(0b1011011, f(0b1011011, 0, &carry));
	BOOST_CHECK(carry);
	BOOST_CHECK_EQUAL(0b1011011, f(0b101101100, 2, &carry));
	BOOST_CHECK(!carry);
	BOOST_CHECK_EQUAL(0, f(0b1011011, 32, &carry));
	BOOST_CHECK(!carry);
	BOOST_CHECK_EQUAL(0x1, f(0x80000000, 31, &carry));
	BOOST_CHECK(!carry);
	BOOST_CHECK_EQUAL(0b1011, f(0b10110000, 4, &carry));
	BOOST_CHECK(!carry);
	BOOST_CHECK_EQUAL(0b1011, f(0b10110, 1, &carry));
	BOOST_CHECK(!carry);
	BOOST_CHECK_EQUAL(0b10, f(0b10110, 3, &carry));
	BOOST_CHECK(carry);
	BOOST_CHECK_EQUAL(0x1, f(0xffffffff, 31, &carry));
	BOOST_CHECK(carry);
	BOOST_CHECK_EQUAL(0x0, f(0xffffffff, 64, &carry));
	BOOST_CHECK(!carry);
	BOOST_CHECK_EQUAL(0x0, f(0x7fffffff, 31, &carry));
	BOOST_CHECK(carry);
	BOOST_CHECK_EQUAL(0x1, f(0x7fffffff, 30, &carry));
	BOOST_CHECK(carry);
	BOOST_CHECK_EQUAL(0x0, f(0x7fffffff, 32, &carry));
	BOOST_CHECK(!carry);
	BOOST_CHECK_EQUAL(0x0, f(0xffffffff, 32, &carry));
	BOOST_CHECK(carry);
}

BOOST_AUTO_TEST_CASE(arithmeticRight)
{
	auto f = Emuballs::Arm::arithmeticRight<uint32_t>;
	bool carry = false;
	BOOST_CHECK_EQUAL(0x1, f(0x8, 3, &carry));
	BOOST_CHECK(!carry);
	BOOST_CHECK_EQUAL(0b1011011, f(0b1011011, 0, &carry));
	BOOST_CHECK(!carry);
	carry = true;
	BOOST_CHECK_EQUAL(0b1011011, f(0b1011011, 0, &carry));
	BOOST_CHECK(carry);
	BOOST_CHECK_EQUAL(0b1011011, f(0b101101100, 2, &carry));
	BOOST_CHECK(!carry);
	BOOST_CHECK_EQUAL(0, f(0b1011011, 32, &carry));
	BOOST_CHECK(!carry);
	BOOST_CHECK_EQUAL(0xffffffff, f(0x80000000, 31, &carry));
	BOOST_CHECK(!carry);
	BOOST_CHECK_EQUAL(0b1011, f(0b10110000, 4, &carry));
	BOOST_CHECK(!carry);
	BOOST_CHECK_EQUAL(0b1011, f(0b10110, 1, &carry));
	BOOST_CHECK(!carry);
	BOOST_CHECK_EQUAL(0b10, f(0b10110, 3, &carry));
	BOOST_CHECK(carry);
	BOOST_CHECK_EQUAL(0xffffffff, f(0xffffffff, 31, &carry));
	BOOST_CHECK(carry);
	BOOST_CHECK_EQUAL(0xffffffff, f(0xffffffff, 64, &carry));
	BOOST_CHECK(carry);
	BOOST_CHECK_EQUAL(0x0, f(0x7fffffff, 31, &carry));
	BOOST_CHECK(carry);
	BOOST_CHECK_EQUAL(0x1, f(0x7fffffff, 30, &carry));
	BOOST_CHECK(carry);
}

BOOST_AUTO_TEST_CASE(rotateRight)
{
	auto f = Emuballs::Arm::rotateRight<uint32_t>;
	bool carry = false;
	BOOST_CHECK_EQUAL(0x1, f(0x8, 3, &carry));
	BOOST_CHECK(!carry);
	BOOST_CHECK_EQUAL(0b1011011, f(0b1011011, 0, &carry));
	BOOST_CHECK(!carry);
	carry = true;
	BOOST_CHECK_EQUAL(0b1011011, f(0b1011011, 0, &carry));
	BOOST_CHECK(carry);
	BOOST_CHECK_EQUAL(0b1011011, f(0b101101100, 2, &carry));
	BOOST_CHECK(!carry);
	BOOST_CHECK_EQUAL(0x7f000000, f(0x7f000000, 32, &carry));
	BOOST_CHECK(!carry);
	BOOST_CHECK_EQUAL(0xff000000, f(0xff000000, 32, &carry));
	BOOST_CHECK(carry);
	BOOST_CHECK_EQUAL(0x7f000000, f(0x7f, 40, &carry));
	BOOST_CHECK(!carry);
	BOOST_CHECK_EQUAL(0x7f000000, f(0x7f, 8, &carry));
	BOOST_CHECK(!carry);
	BOOST_CHECK_EQUAL(0xff000000, f(0xff, 8, &carry));
	BOOST_CHECK(carry);
	BOOST_CHECK_EQUAL(0x1, f(0x80000000, 31, &carry));
	BOOST_CHECK(!carry);
	BOOST_CHECK_EQUAL(0x80000000, f(0x80000000, 32, &carry));
	BOOST_CHECK(carry);
	BOOST_CHECK_EQUAL(0x40000000, f(0x80000000, 33, &carry));
	BOOST_CHECK(!carry);
	BOOST_CHECK_EQUAL(0xc0000000, f(0x80000001, 33, &carry));
	BOOST_CHECK(carry);
	BOOST_CHECK_EQUAL(0b1011, f(0b10110000, 4, &carry));
	BOOST_CHECK(!carry);
	BOOST_CHECK_EQUAL(0b1011, f(0b10110, 1, &carry));
	BOOST_CHECK(!carry);
	BOOST_CHECK_EQUAL(0xc0000002, f(0b10110, 3, &carry));
	BOOST_CHECK(carry);
	BOOST_CHECK_EQUAL(0xffffffff, f(0xffffffff, 31, &carry));
	BOOST_CHECK(carry);
	BOOST_CHECK_EQUAL(0xffffffff, f(0xffffffff, 64, &carry));
	BOOST_CHECK(carry);
	BOOST_CHECK_EQUAL(0xfffffffe, f(0x7fffffff, 31, &carry));
	BOOST_CHECK(carry);
	BOOST_CHECK_EQUAL(0xfffffffd, f(0x7fffffff, 30, &carry));
	BOOST_CHECK(carry);
}

BOOST_AUTO_TEST_CASE(rotateRightExtended)
{
	auto f = Emuballs::Arm::rotateRightExtended<uint32_t>;
	bool carry = false;
	BOOST_CHECK_EQUAL(0b101, f(0b1010, false, &carry));
	BOOST_CHECK(!carry);
	BOOST_CHECK_EQUAL(0b10, f(0b101, false, &carry));
	BOOST_CHECK(carry);
	BOOST_CHECK_EQUAL(0x80000005, f(0b1010, true, &carry));
	BOOST_CHECK(!carry);
	BOOST_CHECK_EQUAL(0x80000002, f(0b101, true, &carry));
	BOOST_CHECK(carry);
}

BOOST_AUTO_TEST_CASE(badShift)
{
	BOOST_CHECK_THROW(Emuballs::Arm::shifter<uint32_t>(500), std::domain_error);
}
