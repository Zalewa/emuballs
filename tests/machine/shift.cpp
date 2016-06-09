#define BOOST_TEST_MODULE shifts
#include <boost/test/unit_test.hpp>
#include "src/machine/shift.hpp"

#include <cstdint>
#include <functional>

BOOST_AUTO_TEST_CASE(logicalLeft)
{
	auto f = Machine::Arm::logicalLeft<uint32_t>;
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
	auto f = Machine::Arm::logicalRight<uint32_t>;
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
	auto f = Machine::Arm::arithmeticRight<uint32_t>;
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
	auto f = Machine::Arm::rotateRight<uint32_t>;
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
	auto f = Machine::Arm::rotateRightExtended<uint32_t>;
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
