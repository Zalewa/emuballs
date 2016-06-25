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
#define BOOST_TEST_MODULE arm_arithmetic_carry_overflow
#include <boost/test/unit_test.hpp>
#include "arm_program_fixture.hpp"
#include <map>
#include <sstream>

using namespace Emuballs::Arm;

struct TestCase
{
	std::string opname;
	regval a;
	regval b;
	regval result;
	char overflow;
	char carry;

	bool isOverflow() const
	{
		return overflow != '_';
	}

	bool isCarry() const
	{
		return carry != '_';
	}
};

std::vector<uint32_t> instructions(const std::string &opname)
{
	if (opname == "adds")
	{
		return {0xe0900001};
	}
	else if (opname == "adcs")
	{
		return {0xe0903001, 0xe0b00001};
	}
	else if (opname == "subs")
	{
		return {0xe0500001};
	}
	else if (opname == "sbcs")
	{
		return {0xe0503001, 0xe0d00001};
	}
	else if (opname == "rsbs")
	{
		return {0xe0700001, 0xe5820000};
	}
	else if (opname == "rscs")
	{
		return {0xe0703001, 0xe0f00001};
	}
	else
		throw std::logic_error("what is op " + opname);
}

void run(const TestCase &testCase)
{
	std::vector<uint32_t> program = {
		0xe5820000, // str	r0, [r2]
		0xe1a0f00e, // mov	pc, lr
	};
	auto extraInstructions = instructions(testCase.opname);
	for (auto it = extraInstructions.rbegin(); it != extraInstructions.rend(); ++it)
		program.insert(program.begin(), *it);
	ArmProgramFixture fixture;
	fixture.load(program);
	fixture.r(0, testCase.a);
	fixture.r(1, testCase.b);
	fixture.runProgram();
	std::stringstream ssBase;
	ssBase << std::hex << testCase.a << " " << testCase.opname << " " << testCase.b
		<< " = " << testCase.result << " c:" << testCase.isCarry()
		<< " v:" << testCase.isOverflow();
	auto &flags = fixture.flags();
	BOOST_CHECK_MESSAGE(flags.carry() == testCase.isCarry(), ssBase.str() + " - carry FAIL");
	BOOST_CHECK_MESSAGE(flags.overflow() == testCase.isOverflow(), ssBase.str() + " - overflow FAIL");
}

void runBatch(const TestCase *begin, const TestCase *end)
{
	for (; begin != end; ++begin)
		run(*begin);
}


BOOST_AUTO_TEST_CASE(arithmetic_carry_overflow)
{
	const TestCase testSet[] = {
		{"adds", 0x7ffffffd, 0x7ffffffd, 0xfffffffa, 'v', '_'},
		{"adds", 0x7ffffffd, 0x7ffffffe, 0xfffffffb, 'v', '_'},
		{"adds", 0x7ffffffd, 0x7fffffff, 0xfffffffc, 'v', '_'},
		{"adds", 0x7ffffffd, 0x80000000, 0xfffffffd, '_', '_'},
		{"adds", 0x7ffffffd, 0x80000001, 0xfffffffe, '_', '_'},
		{"adds", 0x7ffffffd, 0x80000002, 0xffffffff, '_', '_'},
		{"adds", 0x7ffffffd, 0x80000003, 0x0, '_', 'c'},
		{"adds", 0x7ffffffe, 0x7ffffffd, 0xfffffffb, 'v', '_'},
		{"adds", 0x7ffffffe, 0x7ffffffe, 0xfffffffc, 'v', '_'},
		{"adds", 0x7ffffffe, 0x7fffffff, 0xfffffffd, 'v', '_'},
		{"adds", 0x7ffffffe, 0x80000000, 0xfffffffe, '_', '_'},
		{"adds", 0x7ffffffe, 0x80000001, 0xffffffff, '_', '_'},
		{"adds", 0x7ffffffe, 0x80000002, 0x0, '_', 'c'},
		{"adds", 0x7ffffffe, 0x80000003, 0x1, '_', 'c'},
		{"adds", 0x7fffffff, 0x7ffffffd, 0xfffffffc, 'v', '_'},
		{"adds", 0x7fffffff, 0x7ffffffe, 0xfffffffd, 'v', '_'},
		{"adds", 0x7fffffff, 0x7fffffff, 0xfffffffe, 'v', '_'},
		{"adds", 0x7fffffff, 0x80000000, 0xffffffff, '_', '_'},
		{"adds", 0x7fffffff, 0x80000001, 0x0, '_', 'c'},
		{"adds", 0x7fffffff, 0x80000002, 0x1, '_', 'c'},
		{"adds", 0x7fffffff, 0x80000003, 0x2, '_', 'c'},
		{"adds", 0x80000000, 0x7ffffffd, 0xfffffffd, '_', '_'},
		{"adds", 0x80000000, 0x7ffffffe, 0xfffffffe, '_', '_'},
		{"adds", 0x80000000, 0x7fffffff, 0xffffffff, '_', '_'},
		{"adds", 0x80000000, 0x80000000, 0x0, 'v', 'c'},
		{"adds", 0x80000000, 0x80000001, 0x1, 'v', 'c'},
		{"adds", 0x80000000, 0x80000002, 0x2, 'v', 'c'},
		{"adds", 0x80000000, 0x80000003, 0x3, 'v', 'c'},
		{"adds", 0x80000001, 0x7ffffffd, 0xfffffffe, '_', '_'},
		{"adds", 0x80000001, 0x7ffffffe, 0xffffffff, '_', '_'},
		{"adds", 0x80000001, 0x7fffffff, 0x0, '_', 'c'},
		{"adds", 0x80000001, 0x80000000, 0x1, 'v', 'c'},
		{"adds", 0x80000001, 0x80000001, 0x2, 'v', 'c'},
		{"adds", 0x80000001, 0x80000002, 0x3, 'v', 'c'},
		{"adds", 0x80000001, 0x80000003, 0x4, 'v', 'c'},
		{"adds", 0x80000002, 0x7ffffffd, 0xffffffff, '_', '_'},
		{"adds", 0x80000002, 0x7ffffffe, 0x0, '_', 'c'},
		{"adds", 0x80000002, 0x7fffffff, 0x1, '_', 'c'},
		{"adds", 0x80000002, 0x80000000, 0x2, 'v', 'c'},
		{"adds", 0x80000002, 0x80000001, 0x3, 'v', 'c'},
		{"adds", 0x80000002, 0x80000002, 0x4, 'v', 'c'},
		{"adds", 0x80000002, 0x80000003, 0x5, 'v', 'c'},
		{"adds", 0x80000003, 0x7ffffffd, 0x0, '_', 'c'},
		{"adds", 0x80000003, 0x7ffffffe, 0x1, '_', 'c'},
		{"adds", 0x80000003, 0x7fffffff, 0x2, '_', 'c'},
		{"adds", 0x80000003, 0x80000000, 0x3, 'v', 'c'},
		{"adds", 0x80000003, 0x80000001, 0x4, 'v', 'c'},
		{"adds", 0x80000003, 0x80000002, 0x5, 'v', 'c'},
		{"adds", 0x80000003, 0x80000003, 0x6, 'v', 'c'},
		{"adcs", 0x7ffffffd, 0x7ffffffd, 0xfffffffa, 'v', '_'},
		{"adcs", 0x7ffffffd, 0x7ffffffe, 0xfffffffb, 'v', '_'},
		{"adcs", 0x7ffffffd, 0x7fffffff, 0xfffffffc, 'v', '_'},
		{"adcs", 0x7ffffffd, 0x80000000, 0xfffffffd, '_', '_'},
		{"adcs", 0x7ffffffd, 0x80000001, 0xfffffffe, '_', '_'},
		{"adcs", 0x7ffffffd, 0x80000002, 0xffffffff, '_', '_'},
		{"adcs", 0x7ffffffd, 0x80000003, 0x1, '_', 'c'},
		{"adcs", 0x7ffffffe, 0x7ffffffd, 0xfffffffb, 'v', '_'},
		{"adcs", 0x7ffffffe, 0x7ffffffe, 0xfffffffc, 'v', '_'},
		{"adcs", 0x7ffffffe, 0x7fffffff, 0xfffffffd, 'v', '_'},
		{"adcs", 0x7ffffffe, 0x80000000, 0xfffffffe, '_', '_'},
		{"adcs", 0x7ffffffe, 0x80000001, 0xffffffff, '_', '_'},
		{"adcs", 0x7ffffffe, 0x80000002, 0x1, '_', 'c'},
		{"adcs", 0x7ffffffe, 0x80000003, 0x2, '_', 'c'},
		{"adcs", 0x7fffffff, 0x7ffffffd, 0xfffffffc, 'v', '_'},
		{"adcs", 0x7fffffff, 0x7ffffffe, 0xfffffffd, 'v', '_'},
		{"adcs", 0x7fffffff, 0x7fffffff, 0xfffffffe, 'v', '_'},
		{"adcs", 0x7fffffff, 0x80000000, 0xffffffff, '_', '_'},
		{"adcs", 0x7fffffff, 0x80000001, 0x1, '_', 'c'},
		{"adcs", 0x7fffffff, 0x80000002, 0x2, '_', 'c'},
		{"adcs", 0x7fffffff, 0x80000003, 0x3, '_', 'c'},
		{"adcs", 0x80000000, 0x7ffffffd, 0xfffffffd, '_', '_'},
		{"adcs", 0x80000000, 0x7ffffffe, 0xfffffffe, '_', '_'},
		{"adcs", 0x80000000, 0x7fffffff, 0xffffffff, '_', '_'},
		{"adcs", 0x80000000, 0x80000000, 0x1, 'v', 'c'},
		{"adcs", 0x80000000, 0x80000001, 0x2, 'v', 'c'},
		{"adcs", 0x80000000, 0x80000002, 0x3, 'v', 'c'},
		{"adcs", 0x80000000, 0x80000003, 0x4, 'v', 'c'},
		{"adcs", 0x80000001, 0x7ffffffd, 0xfffffffe, '_', '_'},
		{"adcs", 0x80000001, 0x7ffffffe, 0xffffffff, '_', '_'},
		{"adcs", 0x80000001, 0x7fffffff, 0x1, '_', 'c'},
		{"adcs", 0x80000001, 0x80000000, 0x2, 'v', 'c'},
		{"adcs", 0x80000001, 0x80000001, 0x3, 'v', 'c'},
		{"adcs", 0x80000001, 0x80000002, 0x4, 'v', 'c'},
		{"adcs", 0x80000001, 0x80000003, 0x5, 'v', 'c'},
		{"adcs", 0x80000002, 0x7ffffffd, 0xffffffff, '_', '_'},
		{"adcs", 0x80000002, 0x7ffffffe, 0x1, '_', 'c'},
		{"adcs", 0x80000002, 0x7fffffff, 0x2, '_', 'c'},
		{"adcs", 0x80000002, 0x80000000, 0x3, 'v', 'c'},
		{"adcs", 0x80000002, 0x80000001, 0x4, 'v', 'c'},
		{"adcs", 0x80000002, 0x80000002, 0x5, 'v', 'c'},
		{"adcs", 0x80000002, 0x80000003, 0x6, 'v', 'c'},
		{"adcs", 0x80000003, 0x7ffffffd, 0x1, '_', 'c'},
		{"adcs", 0x80000003, 0x7ffffffe, 0x2, '_', 'c'},
		{"adcs", 0x80000003, 0x7fffffff, 0x3, '_', 'c'},
		{"adcs", 0x80000003, 0x80000000, 0x4, 'v', 'c'},
		{"adcs", 0x80000003, 0x80000001, 0x5, 'v', 'c'},
		{"adcs", 0x80000003, 0x80000002, 0x6, 'v', 'c'},
		{"adcs", 0x80000003, 0x80000003, 0x7, 'v', 'c'},
		{"subs", 0x7ffffffd, 0x7ffffffd, 0x0, '_', 'c'},
		{"subs", 0x7ffffffd, 0x7ffffffe, 0xffffffff, '_', '_'},
		{"subs", 0x7ffffffd, 0x7fffffff, 0xfffffffe, '_', '_'},
		{"subs", 0x7ffffffd, 0x80000000, 0xfffffffd, 'v', '_'},
		{"subs", 0x7ffffffd, 0x80000001, 0xfffffffc, 'v', '_'},
		{"subs", 0x7ffffffd, 0x80000002, 0xfffffffb, 'v', '_'},
		{"subs", 0x7ffffffd, 0x80000003, 0xfffffffa, 'v', '_'},
		{"subs", 0x7ffffffe, 0x7ffffffd, 0x1, '_', 'c'},
		{"subs", 0x7ffffffe, 0x7ffffffe, 0x0, '_', 'c'},
		{"subs", 0x7ffffffe, 0x7fffffff, 0xffffffff, '_', '_'},
		{"subs", 0x7ffffffe, 0x80000000, 0xfffffffe, 'v', '_'},
		{"subs", 0x7ffffffe, 0x80000001, 0xfffffffd, 'v', '_'},
		{"subs", 0x7ffffffe, 0x80000002, 0xfffffffc, 'v', '_'},
		{"subs", 0x7ffffffe, 0x80000003, 0xfffffffb, 'v', '_'},
		{"subs", 0x7fffffff, 0x7ffffffd, 0x2, '_', 'c'},
		{"subs", 0x7fffffff, 0x7ffffffe, 0x1, '_', 'c'},
		{"subs", 0x7fffffff, 0x7fffffff, 0x0, '_', 'c'},
		{"subs", 0x7fffffff, 0x80000000, 0xffffffff, 'v', '_'},
		{"subs", 0x7fffffff, 0x80000001, 0xfffffffe, 'v', '_'},
		{"subs", 0x7fffffff, 0x80000002, 0xfffffffd, 'v', '_'},
		{"subs", 0x7fffffff, 0x80000003, 0xfffffffc, 'v', '_'},
		{"subs", 0x80000000, 0x7ffffffd, 0x3, 'v', 'c'},
		{"subs", 0x80000000, 0x7ffffffe, 0x2, 'v', 'c'},
		{"subs", 0x80000000, 0x7fffffff, 0x1, 'v', 'c'},
		{"subs", 0x80000000, 0x80000000, 0x0, '_', 'c'},
		{"subs", 0x80000000, 0x80000001, 0xffffffff, '_', '_'},
		{"subs", 0x80000000, 0x80000002, 0xfffffffe, '_', '_'},
		{"subs", 0x80000000, 0x80000003, 0xfffffffd, '_', '_'},
		{"subs", 0x80000001, 0x7ffffffd, 0x4, 'v', 'c'},
		{"subs", 0x80000001, 0x7ffffffe, 0x3, 'v', 'c'},
		{"subs", 0x80000001, 0x7fffffff, 0x2, 'v', 'c'},
		{"subs", 0x80000001, 0x80000000, 0x1, '_', 'c'},
		{"subs", 0x80000001, 0x80000001, 0x0, '_', 'c'},
		{"subs", 0x80000001, 0x80000002, 0xffffffff, '_', '_'},
		{"subs", 0x80000001, 0x80000003, 0xfffffffe, '_', '_'},
		{"subs", 0x80000002, 0x7ffffffd, 0x5, 'v', 'c'},
		{"subs", 0x80000002, 0x7ffffffe, 0x4, 'v', 'c'},
		{"subs", 0x80000002, 0x7fffffff, 0x3, 'v', 'c'},
		{"subs", 0x80000002, 0x80000000, 0x2, '_', 'c'},
		{"subs", 0x80000002, 0x80000001, 0x1, '_', 'c'},
		{"subs", 0x80000002, 0x80000002, 0x0, '_', 'c'},
		{"subs", 0x80000002, 0x80000003, 0xffffffff, '_', '_'},
		{"subs", 0x80000003, 0x7ffffffd, 0x6, 'v', 'c'},
		{"subs", 0x80000003, 0x7ffffffe, 0x5, 'v', 'c'},
		{"subs", 0x80000003, 0x7fffffff, 0x4, 'v', 'c'},
		{"subs", 0x80000003, 0x80000000, 0x3, '_', 'c'},
		{"subs", 0x80000003, 0x80000001, 0x2, '_', 'c'},
		{"subs", 0x80000003, 0x80000002, 0x1, '_', 'c'},
		{"subs", 0x80000003, 0x80000003, 0x0, '_', 'c'},
		{"sbcs", 0x7ffffffd, 0x7ffffffd, 0x0, '_', 'c'},
		{"sbcs", 0x7ffffffd, 0x7ffffffe, 0xfffffffe, '_', '_'},
		{"sbcs", 0x7ffffffd, 0x7fffffff, 0xfffffffd, '_', '_'},
		{"sbcs", 0x7ffffffd, 0x80000000, 0xfffffffc, 'v', '_'},
		{"sbcs", 0x7ffffffd, 0x80000001, 0xfffffffb, 'v', '_'},
		{"sbcs", 0x7ffffffd, 0x80000002, 0xfffffffa, 'v', '_'},
		{"sbcs", 0x7ffffffd, 0x80000003, 0xfffffff9, 'v', '_'},
		{"sbcs", 0x7ffffffe, 0x7ffffffd, 0x1, '_', 'c'},
		{"sbcs", 0x7ffffffe, 0x7ffffffe, 0x0, '_', 'c'},
		{"sbcs", 0x7ffffffe, 0x7fffffff, 0xfffffffe, '_', '_'},
		{"sbcs", 0x7ffffffe, 0x80000000, 0xfffffffd, 'v', '_'},
		{"sbcs", 0x7ffffffe, 0x80000001, 0xfffffffc, 'v', '_'},
		{"sbcs", 0x7ffffffe, 0x80000002, 0xfffffffb, 'v', '_'},
		{"sbcs", 0x7ffffffe, 0x80000003, 0xfffffffa, 'v', '_'},
		{"sbcs", 0x7fffffff, 0x7ffffffd, 0x2, '_', 'c'},
		{"sbcs", 0x7fffffff, 0x7ffffffe, 0x1, '_', 'c'},
		{"sbcs", 0x7fffffff, 0x7fffffff, 0x0, '_', 'c'},
		{"sbcs", 0x7fffffff, 0x80000000, 0xfffffffe, 'v', '_'},
		{"sbcs", 0x7fffffff, 0x80000001, 0xfffffffd, 'v', '_'},
		{"sbcs", 0x7fffffff, 0x80000002, 0xfffffffc, 'v', '_'},
		{"sbcs", 0x7fffffff, 0x80000003, 0xfffffffb, 'v', '_'},
		{"sbcs", 0x80000000, 0x7ffffffd, 0x3, 'v', 'c'},
		{"sbcs", 0x80000000, 0x7ffffffe, 0x2, 'v', 'c'},
		{"sbcs", 0x80000000, 0x7fffffff, 0x1, 'v', 'c'},
		{"sbcs", 0x80000000, 0x80000000, 0x0, '_', 'c'},
		{"sbcs", 0x80000000, 0x80000001, 0xfffffffe, '_', '_'},
		{"sbcs", 0x80000000, 0x80000002, 0xfffffffd, '_', '_'},
		{"sbcs", 0x80000000, 0x80000003, 0xfffffffc, '_', '_'},
		{"sbcs", 0x80000001, 0x7ffffffd, 0x4, 'v', 'c'},
		{"sbcs", 0x80000001, 0x7ffffffe, 0x3, 'v', 'c'},
		{"sbcs", 0x80000001, 0x7fffffff, 0x2, 'v', 'c'},
		{"sbcs", 0x80000001, 0x80000000, 0x1, '_', 'c'},
		{"sbcs", 0x80000001, 0x80000001, 0x0, '_', 'c'},
		{"sbcs", 0x80000001, 0x80000002, 0xfffffffe, '_', '_'},
		{"sbcs", 0x80000001, 0x80000003, 0xfffffffd, '_', '_'},
		{"sbcs", 0x80000002, 0x7ffffffd, 0x5, 'v', 'c'},
		{"sbcs", 0x80000002, 0x7ffffffe, 0x4, 'v', 'c'},
		{"sbcs", 0x80000002, 0x7fffffff, 0x3, 'v', 'c'},
		{"sbcs", 0x80000002, 0x80000000, 0x2, '_', 'c'},
		{"sbcs", 0x80000002, 0x80000001, 0x1, '_', 'c'},
		{"sbcs", 0x80000002, 0x80000002, 0x0, '_', 'c'},
		{"sbcs", 0x80000002, 0x80000003, 0xfffffffe, '_', '_'},
		{"sbcs", 0x80000003, 0x7ffffffd, 0x6, 'v', 'c'},
		{"sbcs", 0x80000003, 0x7ffffffe, 0x5, 'v', 'c'},
		{"sbcs", 0x80000003, 0x7fffffff, 0x4, 'v', 'c'},
		{"sbcs", 0x80000003, 0x80000000, 0x3, '_', 'c'},
		{"sbcs", 0x80000003, 0x80000001, 0x2, '_', 'c'},
		{"sbcs", 0x80000003, 0x80000002, 0x1, '_', 'c'},
		{"sbcs", 0x80000003, 0x80000003, 0x0, '_', 'c'},
		{"rsbs", 0x7ffffffd, 0x7ffffffd, 0x0, '_', 'c'},
		{"rsbs", 0x7ffffffd, 0x7ffffffe, 0x1, '_', 'c'},
		{"rsbs", 0x7ffffffd, 0x7fffffff, 0x2, '_', 'c'},
		{"rsbs", 0x7ffffffd, 0x80000000, 0x3, 'v', 'c'},
		{"rsbs", 0x7ffffffd, 0x80000001, 0x4, 'v', 'c'},
		{"rsbs", 0x7ffffffd, 0x80000002, 0x5, 'v', 'c'},
		{"rsbs", 0x7ffffffd, 0x80000003, 0x6, 'v', 'c'},
		{"rsbs", 0x7ffffffe, 0x7ffffffd, 0xffffffff, '_', '_'},
		{"rsbs", 0x7ffffffe, 0x7ffffffe, 0x0, '_', 'c'},
		{"rsbs", 0x7ffffffe, 0x7fffffff, 0x1, '_', 'c'},
		{"rsbs", 0x7ffffffe, 0x80000000, 0x2, 'v', 'c'},
		{"rsbs", 0x7ffffffe, 0x80000001, 0x3, 'v', 'c'},
		{"rsbs", 0x7ffffffe, 0x80000002, 0x4, 'v', 'c'},
		{"rsbs", 0x7ffffffe, 0x80000003, 0x5, 'v', 'c'},
		{"rsbs", 0x7fffffff, 0x7ffffffd, 0xfffffffe, '_', '_'},
		{"rsbs", 0x7fffffff, 0x7ffffffe, 0xffffffff, '_', '_'},
		{"rsbs", 0x7fffffff, 0x7fffffff, 0x0, '_', 'c'},
		{"rsbs", 0x7fffffff, 0x80000000, 0x1, 'v', 'c'},
		{"rsbs", 0x7fffffff, 0x80000001, 0x2, 'v', 'c'},
		{"rsbs", 0x7fffffff, 0x80000002, 0x3, 'v', 'c'},
		{"rsbs", 0x7fffffff, 0x80000003, 0x4, 'v', 'c'},
		{"rsbs", 0x80000000, 0x7ffffffd, 0xfffffffd, 'v', '_'},
		{"rsbs", 0x80000000, 0x7ffffffe, 0xfffffffe, 'v', '_'},
		{"rsbs", 0x80000000, 0x7fffffff, 0xffffffff, 'v', '_'},
		{"rsbs", 0x80000000, 0x80000000, 0x0, '_', 'c'},
		{"rsbs", 0x80000000, 0x80000001, 0x1, '_', 'c'},
		{"rsbs", 0x80000000, 0x80000002, 0x2, '_', 'c'},
		{"rsbs", 0x80000000, 0x80000003, 0x3, '_', 'c'},
		{"rsbs", 0x80000001, 0x7ffffffd, 0xfffffffc, 'v', '_'},
		{"rsbs", 0x80000001, 0x7ffffffe, 0xfffffffd, 'v', '_'},
		{"rsbs", 0x80000001, 0x7fffffff, 0xfffffffe, 'v', '_'},
		{"rsbs", 0x80000001, 0x80000000, 0xffffffff, '_', '_'},
		{"rsbs", 0x80000001, 0x80000001, 0x0, '_', 'c'},
		{"rsbs", 0x80000001, 0x80000002, 0x1, '_', 'c'},
		{"rsbs", 0x80000001, 0x80000003, 0x2, '_', 'c'},
		{"rsbs", 0x80000002, 0x7ffffffd, 0xfffffffb, 'v', '_'},
		{"rsbs", 0x80000002, 0x7ffffffe, 0xfffffffc, 'v', '_'},
		{"rsbs", 0x80000002, 0x7fffffff, 0xfffffffd, 'v', '_'},
		{"rsbs", 0x80000002, 0x80000000, 0xfffffffe, '_', '_'},
		{"rsbs", 0x80000002, 0x80000001, 0xffffffff, '_', '_'},
		{"rsbs", 0x80000002, 0x80000002, 0x0, '_', 'c'},
		{"rsbs", 0x80000002, 0x80000003, 0x1, '_', 'c'},
		{"rsbs", 0x80000003, 0x7ffffffd, 0xfffffffa, 'v', '_'},
		{"rsbs", 0x80000003, 0x7ffffffe, 0xfffffffb, 'v', '_'},
		{"rsbs", 0x80000003, 0x7fffffff, 0xfffffffc, 'v', '_'},
		{"rsbs", 0x80000003, 0x80000000, 0xfffffffd, '_', '_'},
		{"rsbs", 0x80000003, 0x80000001, 0xfffffffe, '_', '_'},
		{"rsbs", 0x80000003, 0x80000002, 0xffffffff, '_', '_'},
		{"rsbs", 0x80000003, 0x80000003, 0x0, '_', 'c'},
		{"rscs", 0x7ffffffd, 0x7ffffffd, 0x0, '_', 'c'},
		{"rscs", 0x7ffffffd, 0x7ffffffe, 0x1, '_', 'c'},
		{"rscs", 0x7ffffffd, 0x7fffffff, 0x2, '_', 'c'},
		{"rscs", 0x7ffffffd, 0x80000000, 0x3, 'v', 'c'},
		{"rscs", 0x7ffffffd, 0x80000001, 0x4, 'v', 'c'},
		{"rscs", 0x7ffffffd, 0x80000002, 0x5, 'v', 'c'},
		{"rscs", 0x7ffffffd, 0x80000003, 0x6, 'v', 'c'},
		{"rscs", 0x7ffffffe, 0x7ffffffd, 0xfffffffe, '_', '_'},
		{"rscs", 0x7ffffffe, 0x7ffffffe, 0x0, '_', 'c'},
		{"rscs", 0x7ffffffe, 0x7fffffff, 0x1, '_', 'c'},
		{"rscs", 0x7ffffffe, 0x80000000, 0x2, 'v', 'c'},
		{"rscs", 0x7ffffffe, 0x80000001, 0x3, 'v', 'c'},
		{"rscs", 0x7ffffffe, 0x80000002, 0x4, 'v', 'c'},
		{"rscs", 0x7ffffffe, 0x80000003, 0x5, 'v', 'c'},
		{"rscs", 0x7fffffff, 0x7ffffffd, 0xfffffffd, '_', '_'},
		{"rscs", 0x7fffffff, 0x7ffffffe, 0xfffffffe, '_', '_'},
		{"rscs", 0x7fffffff, 0x7fffffff, 0x0, '_', 'c'},
		{"rscs", 0x7fffffff, 0x80000000, 0x1, 'v', 'c'},
		{"rscs", 0x7fffffff, 0x80000001, 0x2, 'v', 'c'},
		{"rscs", 0x7fffffff, 0x80000002, 0x3, 'v', 'c'},
		{"rscs", 0x7fffffff, 0x80000003, 0x4, 'v', 'c'},
		{"rscs", 0x80000000, 0x7ffffffd, 0xfffffffc, 'v', '_'},
		{"rscs", 0x80000000, 0x7ffffffe, 0xfffffffd, 'v', '_'},
		{"rscs", 0x80000000, 0x7fffffff, 0xfffffffe, 'v', '_'},
		{"rscs", 0x80000000, 0x80000000, 0x0, '_', 'c'},
		{"rscs", 0x80000000, 0x80000001, 0x1, '_', 'c'},
		{"rscs", 0x80000000, 0x80000002, 0x2, '_', 'c'},
		{"rscs", 0x80000000, 0x80000003, 0x3, '_', 'c'},
		{"rscs", 0x80000001, 0x7ffffffd, 0xfffffffb, 'v', '_'},
		{"rscs", 0x80000001, 0x7ffffffe, 0xfffffffc, 'v', '_'},
		{"rscs", 0x80000001, 0x7fffffff, 0xfffffffd, 'v', '_'},
		{"rscs", 0x80000001, 0x80000000, 0xfffffffe, '_', '_'},
		{"rscs", 0x80000001, 0x80000001, 0x0, '_', 'c'},
		{"rscs", 0x80000001, 0x80000002, 0x1, '_', 'c'},
		{"rscs", 0x80000001, 0x80000003, 0x2, '_', 'c'},
		{"rscs", 0x80000002, 0x7ffffffd, 0xfffffffa, 'v', '_'},
		{"rscs", 0x80000002, 0x7ffffffe, 0xfffffffb, 'v', '_'},
		{"rscs", 0x80000002, 0x7fffffff, 0xfffffffc, 'v', '_'},
		{"rscs", 0x80000002, 0x80000000, 0xfffffffd, '_', '_'},
		{"rscs", 0x80000002, 0x80000001, 0xfffffffe, '_', '_'},
		{"rscs", 0x80000002, 0x80000002, 0x0, '_', 'c'},
		{"rscs", 0x80000002, 0x80000003, 0x1, '_', 'c'},
		{"rscs", 0x80000003, 0x7ffffffd, 0xfffffff9, 'v', '_'},
		{"rscs", 0x80000003, 0x7ffffffe, 0xfffffffa, 'v', '_'},
		{"rscs", 0x80000003, 0x7fffffff, 0xfffffffb, 'v', '_'},
		{"rscs", 0x80000003, 0x80000000, 0xfffffffc, '_', '_'},
		{"rscs", 0x80000003, 0x80000001, 0xfffffffd, '_', '_'},
		{"rscs", 0x80000003, 0x80000002, 0xfffffffe, '_', '_'},
		{"rscs", 0x80000003, 0x80000003, 0x0, '_', 'c'},
		{"adds", 0x3, 0x6, 0x9, '_', '_'},
		{"adds", 0xcb, 0x28b, 0x356, '_', '_'},
		{"adds", 0x70000000, 0x70000000, 0xe0000000, 'v', '_'},
		{"subs", 0xb, 0x5, 0x6, '_', 'c'},
		{"subs", 0xb, 0xf, 0xfffffffc, '_', '_'},
	};

	runBatch(std::begin(testSet), std::end(testSet));
}
