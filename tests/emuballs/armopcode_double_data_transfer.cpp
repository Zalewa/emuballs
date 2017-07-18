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
#define BOOST_TEST_MODULE armopcode_double_data_transfer
#include <boost/test/unit_test.hpp>
#include "src/emuballs/armopcode_impl.hpp"
#include "src/emuballs/errors.hpp"
#include "arm_program_fixture.hpp"

struct Fixture : public ArmProgramFixture
{
	const size_t MEM_BASE;
	const size_t MEM_RANGE;

	Fixture() : MEM_BASE(0x40000), MEM_RANGE(0x80)
	{
		auto &memory = machine.memory();
		for (auto addr = -(MEM_RANGE / 2);  addr < (MEM_RANGE / 2); addr += sizeof(uint32_t))
			memory.putWord(addr, 0xccc4cccc * addr);
	}
};

constexpr uint64_t memval = 0xc331a1b9cafebeba;
constexpr uint32_t memvalLow = 0xcafebeba;
constexpr uint32_t memvalHigh = 0xc331a1b9;

static Emuballs::Arm::OpcodePtr decode(uint32_t code)
{
	return Emuballs::Arm::opcodeDoublewordDataTransfer(code);
}

BOOST_FIXTURE_TEST_SUITE(suite, Fixture)

BOOST_AUTO_TEST_CASE(ldrd)
{
	auto op = decode(0xe1c200d0); // ldrd r0, [r2]
	machine.memory().putDword(MEM_BASE, memval);
	r(0, 0);
	r(1, 0);
	r(2, MEM_BASE);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), memvalLow);
	BOOST_CHECK_EQUAL(r(1), memvalHigh);
	BOOST_CHECK_EQUAL(r(2), MEM_BASE);
}

BOOST_AUTO_TEST_CASE(ldrd_pre_indexed_imm)
{
	auto op = decode(0xe1c200d4); // ldrd r0, [r2, #4]
	machine.memory().putDword(MEM_BASE + 4, memval);
	r(0, 0);
	r(1, 0);
	r(2, MEM_BASE);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), memvalLow);
	BOOST_CHECK_EQUAL(r(1), memvalHigh);
	BOOST_CHECK_EQUAL(r(2), MEM_BASE);

	reset();
	auto opNegative = decode(0xe14200d8); // ldrd r0, [r2, -#8]
	machine.memory().putDword(MEM_BASE - 8, memval + 0x0b0b0b0b0f0f0f0f);
	r(0, 0);
	r(1, 0);
	r(2, MEM_BASE);
	opNegative->execute(machine);
	BOOST_CHECK_EQUAL(r(0), memvalLow + 0x0f0f0f0f);
	BOOST_CHECK_EQUAL(r(1), memvalHigh + 0x0b0b0b0b);
	BOOST_CHECK_EQUAL(r(2), MEM_BASE);
}

BOOST_AUTO_TEST_CASE(ldrd_pre_indexed_imm_writeback)
{
	auto op = decode(0xe1e280d4); // ldrd r8, [r2, #4]!
	machine.memory().putDword(MEM_BASE + 4, memval);
	r(8, 0);
	r(9, 0);
	r(2, MEM_BASE);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(8), memvalLow);
	BOOST_CHECK_EQUAL(r(9), memvalHigh);
	BOOST_CHECK_EQUAL(r(2), MEM_BASE + 4);

	reset();
	auto opNegative = decode(0xe16280d8); // ldrd r8, [r2, -#8]!
	machine.memory().putDword(MEM_BASE - 8, memval + 0x0b0b0b0b0f0f0f0f);
	r(8, 0);
	r(9, 0);
	r(2, MEM_BASE);
	opNegative->execute(machine);
	BOOST_CHECK_EQUAL(r(8), memvalLow + 0x0f0f0f0f);
	BOOST_CHECK_EQUAL(r(9), memvalHigh + 0x0b0b0b0b);
	BOOST_CHECK_EQUAL(r(2), MEM_BASE - 8);
}

BOOST_AUTO_TEST_CASE(ldrd_post_indexed_imm)
{
	auto op = decode(0xe0c208d4); // ldrd r0, [r2], #0x84
	machine.memory().putDword(MEM_BASE, memval);
	machine.memory().putDword(MEM_BASE + 0x84, memval * 4); // something else
	r(0, 0);
	r(1, 0);
	r(2, MEM_BASE);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), memvalLow);
	BOOST_CHECK_EQUAL(r(1), memvalHigh);
	BOOST_CHECK_EQUAL(r(2), MEM_BASE + 0x84);

	reset();
	auto opNegative = decode(0xe04200dc); // ldrd r0, [r1], #-12
	machine.memory().putDword(MEM_BASE, memval);
	machine.memory().putDword(MEM_BASE - 12, memval - 0x0f0f0f0f);
	r(0, 0);
	r(1, 0);
	r(2, MEM_BASE);
	opNegative->execute(machine);
	BOOST_CHECK_EQUAL(r(0), memvalLow);
	BOOST_CHECK_EQUAL(r(1), memvalHigh);
	BOOST_CHECK_EQUAL(r(2), MEM_BASE - 12);
}

BOOST_AUTO_TEST_CASE(ldrd_pre_indexed_register)
{
	auto op = decode(0xe18140d2); // ldrd r4, [r1, r2]
	machine.memory().putDword(MEM_BASE + 12, memval);
	r(4, 0);
	r(5, 0);
	r(1, MEM_BASE);
	r(2, 12);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(4), memvalLow);
	BOOST_CHECK_EQUAL(r(5), memvalHigh);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE);
	BOOST_CHECK_EQUAL(r(2), 12U);

	reset();
	auto opNegative = decode(0xe10140d2); // ldrd r4, [r1, -r2]
	machine.memory().putDword(MEM_BASE - 16, memval + 0xcafebeef1a2b3c4d);
	r(4, 0);
	r(5, 0);
	r(1, MEM_BASE);
	r(2, 16);
	opNegative->execute(machine);
	BOOST_CHECK_EQUAL(r(4), memvalLow + 0x1a2b3c4d);
	BOOST_CHECK_EQUAL(r(5), memvalHigh + 0xcafebeef);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE);
	BOOST_CHECK_EQUAL(r(2), 16U);

	auto opZero = decode(0xe18140d2); // ldrd r4, [r1, r2]
	machine.memory().putDword(MEM_BASE, memval);
	r(4, 0);
	r(5, 0);
	r(1, MEM_BASE);
	r(2, 0);
	opZero->execute(machine);
	BOOST_CHECK_EQUAL(r(4), memvalLow);
	BOOST_CHECK_EQUAL(r(5), memvalHigh);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE);
	BOOST_CHECK_EQUAL(r(2), 0U);
}

BOOST_AUTO_TEST_CASE(strd)
{
	auto op = decode(0xe1c200fc); // strd r0, [r2, #12]
	machine.memory().putDword(MEM_BASE, 0);
	r(0, memvalLow);
	r(1, memvalHigh);
	r(2, MEM_BASE);
	op->execute(machine);
	BOOST_CHECK_EQUAL(machine.memory().dword(MEM_BASE + 12), memval);
	BOOST_CHECK_EQUAL(r(2), MEM_BASE);
}

BOOST_AUTO_TEST_CASE(r15_write_back)
{
	using namespace Emuballs;
	BOOST_CHECK_THROW(decode(0xe1ef80d4)->validate(), IllegalOpcodeError); // ldrd r8, [r15, #4]!
	BOOST_CHECK_NO_THROW(decode(0xe1e280d4)->validate()); // ldrd r8, [r2, #4]!
	BOOST_CHECK_NO_THROW(decode(0xe14f00d8)->validate()); // ldrd r0, [r15, -#8]
}

BOOST_AUTO_TEST_CASE(r_odd)
{
	using namespace Emuballs;
	BOOST_CHECK_NO_THROW(decode(0xe1c200d0)->validate()); // ldrd r0, [r2]
	BOOST_CHECK_THROW(decode(0xe1c230d0)->validate(), IllegalOpcodeError); // ldrd r3, [r2]
}

BOOST_AUTO_TEST_SUITE_END()
