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
#define BOOST_TEST_MODULE armopcode_block_data_transfer
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
		auto &memory = machine.untrackedMemory();
		for (auto addr = -(MEM_RANGE / 2);  addr < (MEM_RANGE / 2); addr += sizeof(uint32_t))
			memory.putWord(addr, 0xccc4cccc * addr);
	}
};

constexpr auto memval = 0xc331a1b9;

static Emuballs::Arm::OpcodePtr decode(uint32_t code)
{
	return Emuballs::Arm::opcodeBlockDataTransfer(code);
}

BOOST_FIXTURE_TEST_SUITE(suite, Fixture)

BOOST_AUTO_TEST_CASE(ldm_postindex_up)
{
	auto op = decode(0xe892000a); // ldm r2, {r1, r3}
	machine.memory().putWord(MEM_BASE, 0xcafebeba);
	machine.memory().putWord(MEM_BASE + 4, 0xdeadbeef);
	r(1, 0);
	r(2, MEM_BASE);
	r(3, 0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(1), 0xcafebeba);
	BOOST_CHECK_EQUAL(r(2), MEM_BASE);
	BOOST_CHECK_EQUAL(r(3), 0xdeadbeef);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE), 0xcafebeba);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE + 4), 0xdeadbeef);
}

BOOST_AUTO_TEST_CASE(ldm_postindex_up_writeback)
{
	auto op = decode(0xe8b2000a); // ldm r2!, {r1, r3}
	machine.memory().putWord(MEM_BASE, 0xcafebeba);
	machine.memory().putWord(MEM_BASE + 4, 0xdeadbeef);
	r(1, 0);
	r(2, MEM_BASE);
	r(3, 0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(1), 0xcafebeba);
	BOOST_CHECK_EQUAL(r(2), MEM_BASE + 8);
	BOOST_CHECK_EQUAL(r(3), 0xdeadbeef);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE), 0xcafebeba);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE + 4), 0xdeadbeef);
}

BOOST_AUTO_TEST_CASE(ldm_postindex_up_spsr)
{
	auto op = decode(0xe8d2000a); // ldm r2, {r1, r3}^
	machine.memory().putWord(MEM_BASE, 0xcafebeba);
	machine.memory().putWord(MEM_BASE + 4, 0xdeadbeef);
	r(1, 0);
	r(2, MEM_BASE);
	r(3, 0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(1), 0xcafebeba);
	BOOST_CHECK_EQUAL(r(2), MEM_BASE);
	BOOST_CHECK_EQUAL(r(3), 0xdeadbeef);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE), 0xcafebeba);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE + 4), 0xdeadbeef);
}

BOOST_AUTO_TEST_CASE(ldm_postindex_up_writeback_self)
{
	auto op = decode(0xe8b500f8); // ldm r5!, {r3, r4, r5, r6, r7}
	int i = 0;
	for (auto value : {0xdeadbeef, 0xcafebeba, 1u, 0xa1b2c4d4, 0xfedcba98})
	{
		machine.memory().putWord(MEM_BASE + (i * 4), value);
		++i;
	}
	r(3, 0);
	r(4, 0);
	r(5, MEM_BASE);
	r(6, 0);
	r(7, 0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(3), 0xdeadbeef);
	BOOST_CHECK_EQUAL(r(4), 0xcafebeba);
	// ARM architecture reference manual (1996-2000) (arm.pdf):
	// If the base register <Rn> is specified in <registers>, and base register
	// writeback is specified, the final value of <Rn> is UNPREDICTABLE .
}

BOOST_AUTO_TEST_CASE(ldm_postindex_up_self)
{
	auto op = decode(0xe89500f8); // ldm r5, {r3, r4, r5, r6, r7}
	int i = 0;
	for (auto value : {0xdeadbeef, 0xcafebeba, 1u, 0xa1b2c4d4, 0xfedcba98})
	{
		machine.memory().putWord(MEM_BASE + (i * 4), value);
		++i;
	}
	r(3, 0);
	r(4, 0);
	r(5, MEM_BASE);
	r(6, 0);
	r(7, 0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(3), 0xdeadbeef);
	BOOST_CHECK_EQUAL(r(4), 0xcafebeba);
	BOOST_CHECK_EQUAL(r(5), 1u);
	BOOST_CHECK_EQUAL(r(6), 0xa1b2c4d4);
	BOOST_CHECK_EQUAL(r(7), 0xfedcba98);
}

BOOST_AUTO_TEST_CASE(ldm_preindex_up)
{
	auto op = decode(0xe992000a); // ldmib r2, {r1, r3}
	machine.memory().putWord(MEM_BASE, 0x80706050);
	machine.memory().putWord(MEM_BASE + 4, 0x12345678u);
	machine.memory().putWord(MEM_BASE + 8, 0x87ab45de);
	r(1, 0);
	r(2, MEM_BASE);
	r(3, 0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(1), 0x12345678u);
	BOOST_CHECK_EQUAL(r(2), MEM_BASE);
	BOOST_CHECK_EQUAL(r(3), 0x87ab45de);
}

BOOST_AUTO_TEST_CASE(ldm_preindex_up_writeback)
{
	auto op = decode(0xe9b2000a); // ldmib r2!, {r1, r3}
	machine.memory().putWord(MEM_BASE, 0x80706050);
	machine.memory().putWord(MEM_BASE + 4, 0x12345678u);
	machine.memory().putWord(MEM_BASE + 8, 0x87ab45de);
	r(1, 0);
	r(2, MEM_BASE);
	r(3, 0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(1), 0x12345678u);
	BOOST_CHECK_EQUAL(r(2), MEM_BASE + 8);
	BOOST_CHECK_EQUAL(r(3), 0x87ab45de);
}

BOOST_AUTO_TEST_CASE(ldm_preindex_down_writeback)
{
	auto op = decode(0xe932000a); // ldmdb r2!, {r1, r3}
	machine.memory().putWord(MEM_BASE, 0x80706050);
	machine.memory().putWord(MEM_BASE - 4, 0x12345678u);
	machine.memory().putWord(MEM_BASE - 8, 0x87ab45de);
	r(1, 0);
	r(2, MEM_BASE);
	r(3, 0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(1), 0x87ab45de);
	BOOST_CHECK_EQUAL(r(2), MEM_BASE - 8);
	BOOST_CHECK_EQUAL(r(3), 0x12345678u);
}

BOOST_AUTO_TEST_CASE(ldm_postindex_down_writeback)
{
	auto op = decode(0xe832000a); // ldmda r2!, {r1, r3}
	machine.memory().putWord(MEM_BASE, 0x80706050);
	machine.memory().putWord(MEM_BASE - 4, 0x12345678u);
	machine.memory().putWord(MEM_BASE - 8, 0x87ab45de);
	r(1, 0);
	r(2, MEM_BASE);
	r(3, 0);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(1), 0x12345678u);
	BOOST_CHECK_EQUAL(r(2), MEM_BASE - 8);
	BOOST_CHECK_EQUAL(r(3), 0x80706050);
}

//////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(stm_postindex_up)
{
	auto op = decode(0xe882000a); // stm r2, {r1, r3}
	machine.memory().putWord(MEM_BASE, 0);
	machine.memory().putWord(MEM_BASE + 4, 0);
	r(1, 0xcafebeba);
	r(2, MEM_BASE);
	r(3, 0xdeadbeef);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(1), 0xcafebeba);
	BOOST_CHECK_EQUAL(r(2), MEM_BASE);
	BOOST_CHECK_EQUAL(r(3), 0xdeadbeef);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE), 0xcafebeba);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE + 4), 0xdeadbeef);
}

BOOST_AUTO_TEST_CASE(stm_postindex_up_writeback)
{
	auto op = decode(0xe8a2000a); // stmia r2!, {r1, r3}
	machine.memory().putWord(MEM_BASE, 0);
	machine.memory().putWord(MEM_BASE + 4, 0);
	r(1, 0xcafebeba);
	r(2, MEM_BASE);
	r(3, 0xdeadbeef);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(1), 0xcafebeba);
	BOOST_CHECK_EQUAL(r(2), MEM_BASE + 8);
	BOOST_CHECK_EQUAL(r(3), 0xdeadbeef);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE), 0xcafebeba);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE + 4), 0xdeadbeef);
}

BOOST_AUTO_TEST_CASE(stm_postindex_up_spsr)
{
	auto op = decode(0xe8c2000a); // stmia r2, {r1, r3}^
	machine.memory().putWord(MEM_BASE, 0);
	machine.memory().putWord(MEM_BASE + 4, 0);
	r(1, 0xcafebeba);
	r(2, MEM_BASE);
	r(3, 0xdeadbeef);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(1), 0xcafebeba);
	BOOST_CHECK_EQUAL(r(2), MEM_BASE);
	BOOST_CHECK_EQUAL(r(3), 0xdeadbeef);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE), 0xcafebeba);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE + 4), 0xdeadbeef);
}

BOOST_AUTO_TEST_CASE(stm_postindex_up_writeback_self_first)
{
	auto op = decode(0xe8a300f8); // stmia r3!, {r3, r4, r5, r6, r7}
	for (int i = 0; i < 5; ++i)
		machine.memory().putWord(MEM_BASE + (i * 4), 0);
	r(3, MEM_BASE);
	r(4, 0xcafebeba);
	r(5, 0xdeadbeef);
	r(6, 0xa1b2c3d4);
	r(7, 0xfedcba98);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(3), MEM_BASE + 20);
	BOOST_CHECK_EQUAL(r(4), 0xcafebeba);
	BOOST_CHECK_EQUAL(r(5), 0xdeadbeef);
	BOOST_CHECK_EQUAL(r(6), 0xa1b2c3d4);
	BOOST_CHECK_EQUAL(r(7), 0xfedcba98);
	int i = 0;
	uint32_t values[] = {static_cast<uint32_t>(MEM_BASE), 0xcafebeba, 0xdeadbeef, 0xa1b2c3d4, 0xfedcba98};
	for (uint32_t value : values)
	{
		BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE + (i * 4)), value);
		++i;
	}
}

BOOST_AUTO_TEST_CASE(stm_postindex_up_writeback_self_notfirst)
{
	auto op = decode(0xe8a500f8); // stmia r5!, {r3, r4, r5, r6, r7}
	for (int i = 0; i < 5; ++i)
		machine.memory().putWord(MEM_BASE + (i * 4), 0);
	r(3, 0xdeadbeef);
	r(4, 0xcafebeba);
	r(5, MEM_BASE);
	r(6, 0xa1b2c3d4);
	r(7, 0xfedcba98);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(3), 0xdeadbeef);
	BOOST_CHECK_EQUAL(r(4), 0xcafebeba);
	// If Rn is to be in the list, it must be the lowest register on the list.
	int i = 0;
	for (auto value : {0xdeadbeef, 0xcafebeba})
	{
		BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE + (i * 4)), value);
		++i;
	}
}

BOOST_AUTO_TEST_CASE(stm_preindex_up)
{
	auto op = decode(0xe982000a); // stmib r2, {r1, r3}
	machine.memory().putWord(MEM_BASE, 1);
	machine.memory().putWord(MEM_BASE + 4, 2);
	machine.memory().putWord(MEM_BASE + 8, 3);
	r(1, 0x80706050);
	r(2, MEM_BASE);
	r(3, 0x12345678);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(1), 0x80706050);
	BOOST_CHECK_EQUAL(r(2), MEM_BASE);
	BOOST_CHECK_EQUAL(r(3), 0x12345678u);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE), 1u);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE + 4), 0x80706050);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE + 8), 0x12345678u);
}

BOOST_AUTO_TEST_CASE(stm_preindex_down)
{
	auto op = decode(0xe902000a); // stmdb r2, {r1, r3}
	machine.memory().putWord(MEM_BASE, 1);
	machine.memory().putWord(MEM_BASE - 4, 2);
	machine.memory().putWord(MEM_BASE - 8, 3);
	r(1, 0x80706050);
	r(2, MEM_BASE);
	r(3, 0x12345678u);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(1), 0x80706050);
	BOOST_CHECK_EQUAL(r(2), MEM_BASE);
	BOOST_CHECK_EQUAL(r(3), 0x12345678u);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE), 1u);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE - 4), 0x12345678u);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE - 8), 0x80706050);
}

BOOST_AUTO_TEST_CASE(stm_postindex_down)
{
	auto op = decode(0xe802000a); // stmda r2, {r1, r3}
	machine.memory().putWord(MEM_BASE, 1);
	machine.memory().putWord(MEM_BASE - 4, 2);
	machine.memory().putWord(MEM_BASE - 8, 3);
	r(1, 0x80706050);
	r(2, MEM_BASE);
	r(3, 0x12345678);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(1), 0x80706050);
	BOOST_CHECK_EQUAL(r(2), MEM_BASE);
	BOOST_CHECK_EQUAL(r(3), 0x12345678u);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE), 0x12345678u);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE - 4), 0x80706050);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE - 8), 3u);
}

BOOST_AUTO_TEST_CASE(r15_base)
{
	using namespace Emuballs;
	// ldm r15, {r0, r1, r2, r3, r4, r5, r6, r7}
	BOOST_CHECK_THROW(decode(0xe89f00ff)->validate(), IllegalOpcodeError);
	// stm r15, {r0, r1, r2, r3, r4, r5, r6, r7}
	BOOST_CHECK_THROW(decode(0xe88f00ff)->validate(), IllegalOpcodeError);
	// ldm r14, {r0, r1, r2, r3, r4, r5, r6, r7}
	BOOST_CHECK_NO_THROW(decode(0xe89e00ff)->validate());
	// stm r14, {r0, r1, r2, r3, r4, r5, r6, r7}
	BOOST_CHECK_NO_THROW(decode(0xe88e00ff)->validate());
}


BOOST_AUTO_TEST_SUITE_END()
