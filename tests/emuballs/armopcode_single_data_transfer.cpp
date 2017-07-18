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
#define BOOST_TEST_MODULE armopcode_single_data_transfer
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
	return Emuballs::Arm::opcodeSingleDataTransfer(code);
}

BOOST_FIXTURE_TEST_SUITE(suite, Fixture)


BOOST_AUTO_TEST_CASE(ldr)
{
	auto op = decode(0xe5910000); // ldr r0, [r1]
	machine.memory().putWord(MEM_BASE, memval);
	r(0, 0);
	r(1, MEM_BASE);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), memval);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE);
}

BOOST_AUTO_TEST_CASE(ldr_pre_indexed_imm)
{
	auto op = decode(0xe5910004); // ldr r0, [r1, #4]
	machine.memory().putWord(MEM_BASE + 4, memval);
	r(0, 0);
	r(1, MEM_BASE);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), memval);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE);

	reset();
	auto opNegative = decode(0xe5110004); // ldr r0, [r1, #-4]
	machine.memory().putWord(MEM_BASE - 4, memval + 0x0f0f0f0f);
	r(0, 0);
	r(1, MEM_BASE);
	opNegative->execute(machine);
	BOOST_CHECK_EQUAL(r(0), memval + 0x0f0f0f0f);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE);
}

BOOST_AUTO_TEST_CASE(ldr_pre_indexed_imm_writeback)
{
	auto op = decode(0xe5b10004); // ldr r0, [r1, #4]!
	machine.memory().putWord(MEM_BASE + 4, memval);
	r(0, 0);
	r(1, MEM_BASE);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), memval);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE + 4);

	reset();
	auto opNegative = decode(0xe5310004); // ldr r0, [r1, #-4]!
	machine.memory().putWord(MEM_BASE - 4, memval + 0x0f0f0f0f);
	r(0, 0);
	r(1, MEM_BASE);
	opNegative->execute(machine);
	BOOST_CHECK_EQUAL(r(0), memval + 0x0f0f0f0f);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE - 4);
}

BOOST_AUTO_TEST_CASE(ldr_post_indexed_imm)
{
	auto op = decode(0xe4910004); // ldr r0, [r1], #4
	machine.memory().putWord(MEM_BASE, memval);
	machine.memory().putWord(MEM_BASE + 4, memval + 0x0f0f0f0f);
	r(0, 0);
	r(1, MEM_BASE);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), memval);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE + 4);

	reset();
	auto opNegative = decode(0xe4110004); // ldr r0, [r1], #-4
	machine.memory().putWord(MEM_BASE, memval + 0xc4c4c4c4);
	machine.memory().putWord(MEM_BASE - 4, memval - 0x0f0f0f0f);
	r(0, 0);
	r(1, MEM_BASE);
	opNegative->execute(machine);
	BOOST_CHECK_EQUAL(r(0), memval + 0xc4c4c4c4);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE - 4);
}

BOOST_AUTO_TEST_CASE(ldr_pre_indexed_register)
{
	auto op = decode(0xe7910002); // ldr r0, [r1, r2]
	machine.memory().putWord(MEM_BASE + 12, memval);
	r(0, 0);
	r(1, MEM_BASE);
	r(2, 12);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), memval);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE);

	reset();
	auto opNegative = decode(0xe7110002); // ldr r0, [r1, -r2]
	machine.memory().putWord(MEM_BASE - 12, memval + 0x0f0f0f0f);
	r(0, 0);
	r(1, MEM_BASE);
	r(2, 12);
	opNegative->execute(machine);
	BOOST_CHECK_EQUAL(r(0), memval + 0x0f0f0f0f);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE);
}

BOOST_AUTO_TEST_CASE(ldr_pre_indexed_register_shifted)
{
	auto op = decode(0xe7910282); // ldr r0, [r1, r2, lsl #5]
	machine.memory().putWord(MEM_BASE + (12 << 5), memval);
	r(0, 0);
	r(1, MEM_BASE);
	r(2, 12);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), memval);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE);

	reset();
	auto opNegative = decode(0xe7110842); // ldr r0, [r1, -r2, asr #16]
	machine.memory().putWord(MEM_BASE + 12, memval + 0x0f0f0f0f);
	r(0, 0);
	r(1, MEM_BASE);
	r(2, -0xc0000); // should give -12
	opNegative->execute(machine);
	BOOST_CHECK_EQUAL(r(0), memval + 0x0f0f0f0f);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE);
}

BOOST_AUTO_TEST_CASE(ldr_post_indexed_register)
{
	auto op = decode(0xe6910002); // ldr r0, [r1], r2
	machine.memory().putWord(MEM_BASE, memval);
	machine.memory().putWord(MEM_BASE + 8, memval + 0x0f0f0f0f);
	r(0, 0);
	r(1, MEM_BASE);
	r(2, 8);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), memval);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE + 8);

	reset();
	auto opNegative = decode(0xe6110002); // ldr r0, [r1], -r2
	machine.memory().putWord(MEM_BASE, memval + 0xc4c4c4c4);
	machine.memory().putWord(MEM_BASE - 16, memval - 0x0f0f0f0f);
	r(0, 0);
	r(1, MEM_BASE);
	r(2, 16);
	opNegative->execute(machine);
	BOOST_CHECK_EQUAL(r(0), memval + 0xc4c4c4c4);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE - 16);
}

BOOST_AUTO_TEST_CASE(ldr_byte)
{
	auto op = decode(0xe5d10000); // ldrb r0, [r1]
	machine.memory().putWord(MEM_BASE, 0xa1b2c3d4);
	r(0, 0);
	r(1, MEM_BASE);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xd4);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE);

	r(0, 0);
	r(1, MEM_BASE + 1);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xc3);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE + 1);

	r(0, 0);
	r(1, MEM_BASE + 2);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xb2);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE + 2);

	r(0, 0);
	r(1, MEM_BASE + 3);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xa1);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE + 3);
}

BOOST_AUTO_TEST_CASE(str)
{
	auto op = decode(0xe5810000); // str r0, [r1]
	machine.memory().putWord(MEM_BASE, 0);
	r(0, memval);
	r(1, MEM_BASE);
	op->execute(machine);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE), memval);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE);
}

BOOST_AUTO_TEST_CASE(str_pre_indexed_imm)
{
	auto op = decode(0xe5810004); // str r0, [r1, #4]
	machine.memory().putWord(MEM_BASE + 4, 0);
	r(0, memval);
	r(1, MEM_BASE);
	op->execute(machine);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE + 4), memval);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE);

	reset();
	auto opNegative = decode(0xe5010004); // str r0, [r1, #-4]
	machine.memory().putWord(MEM_BASE - 4, -1);
	r(0, memval + 0x0f0f0f0f);
	r(1, MEM_BASE);
	opNegative->execute(machine);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE - 4), memval + 0x0f0f0f0f);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE);
}

BOOST_AUTO_TEST_CASE(str_pre_indexed_imm_writeback)
{
	auto op = decode(0xe5a10004); // str r0, [r1, #4]!
	machine.memory().putWord(MEM_BASE + 4, 0);
	r(0, memval);
	r(1, MEM_BASE);
	op->execute(machine);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE + 4), memval);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE + 4);

	reset();
	auto opNegative = decode(0xe5210004); // str r0, [r1, #-4]!
	machine.memory().putWord(MEM_BASE - 4, -1);
	r(0, memval + 0x0f0f0f0f);
	r(1, MEM_BASE);
	opNegative->execute(machine);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE - 4), memval + 0x0f0f0f0f);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE - 4);
}

BOOST_AUTO_TEST_CASE(str_post_indexed_imm)
{
	auto op = decode(0xe4810004); // str r0, [r1], #4
	machine.memory().putWord(MEM_BASE, 0);
	machine.memory().putWord(MEM_BASE + 4, 0x0f0f0f0f);
	r(0, memval);
	r(1, MEM_BASE);
	op->execute(machine);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE), memval);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE + 4);

	reset();
	auto opNegative = decode(0xe4010004); // str r0, [r1], #-4
	machine.memory().putWord(MEM_BASE, 0xc4c4c4c4);
	machine.memory().putWord(MEM_BASE - 4, memval - 0x0f0f0f0f);
	r(0, memval + 0xc4c4c4c4);
	r(1, MEM_BASE);
	opNegative->execute(machine);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE), memval + 0xc4c4c4c4);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE - 4);
}

BOOST_AUTO_TEST_CASE(str_pre_indexed_register)
{
	auto op = decode(0xe7810002); // str r0, [r1, r2]
	machine.memory().putWord(MEM_BASE + 12, 0);
	r(0, memval);
	r(1, MEM_BASE);
	r(2, 12);
	op->execute(machine);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE + 12), memval);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE);

	reset();
	auto opNegative = decode(0xe7010002); // str r0, [r1, -r2]
	machine.memory().putWord(MEM_BASE - 12, 0);
	r(0, memval + 0x0f0f0f0f);
	r(1, MEM_BASE);
	r(2, 12);
	opNegative->execute(machine);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE - 12), memval + 0x0f0f0f0f);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE);
}

BOOST_AUTO_TEST_CASE(str_pre_indexed_register_shifted)
{
	auto op = decode(0xe7810282); // str r0, [r1, r2, lsl #5]
	machine.memory().putWord(MEM_BASE + (12 << 5), 0);
	r(0, memval);
	r(1, MEM_BASE);
	r(2, 12);
	op->execute(machine);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE + (12 << 5)), memval);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE);

	reset();
	auto opNegative = decode(0xe7010842); // str r0, [r1, -r2, asr #16]
	machine.memory().putWord(MEM_BASE + 12, 0);
	r(0, memval + 0x0f0f0f0f);
	r(1, MEM_BASE);
	r(2, -0xc0000); // should give -12
	opNegative->execute(machine);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE + 12), memval + 0x0f0f0f0f);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE);
}

BOOST_AUTO_TEST_CASE(str_post_indexed_register)
{
	auto op = decode(0xe6810002); // str r0, [r1], r2
	machine.memory().putWord(MEM_BASE, 0);
	machine.memory().putWord(MEM_BASE + 8, 0x0f0f0f0f);
	r(0, memval);
	r(1, MEM_BASE);
	r(2, 8);
	op->execute(machine);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE), memval);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE + 8);

	reset();
	auto opNegative = decode(0xe6010002); // str r0, [r1], -r2
	machine.memory().putWord(MEM_BASE, 0x93939393);
	machine.memory().putWord(MEM_BASE - 16, 0x0f0f0f0f);
	r(0, memval + 0xc4c4c4c4);
	r(1, MEM_BASE);
	r(2, 16);
	opNegative->execute(machine);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_BASE), memval + 0xc4c4c4c4);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE - 16);
}

BOOST_AUTO_TEST_CASE(str_byte)
{
	auto op = decode(0xe5c10000); // strb r0, [r1]
	machine.memory().putWord(MEM_BASE, 0xcafebeeb);
	r(0, 0xa1);
	r(1, MEM_BASE);
	op->execute(machine);
	BOOST_CHECK_EQUAL(machine.memory().byte(MEM_BASE), 0xa1);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE);

	r(0, 0xb2);
	r(1, MEM_BASE + 1);
	op->execute(machine);
	BOOST_CHECK_EQUAL(machine.memory().byte(MEM_BASE + 1), 0xb2);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE + 1);

	r(0, 0xc3);
	r(1, MEM_BASE + 2);
	op->execute(machine);
	BOOST_CHECK_EQUAL(machine.memory().byte(MEM_BASE + 2), 0xc3);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE + 2);

	r(0, 0xd4);
	r(1, MEM_BASE + 3);
	op->execute(machine);
	BOOST_CHECK_EQUAL(machine.memory().byte(MEM_BASE + 3), 0xd4);
	BOOST_CHECK_EQUAL(r(1), MEM_BASE + 3);
}

BOOST_AUTO_TEST_CASE(r15_write_back)
{
	using namespace Emuballs;
	BOOST_CHECK_THROW(decode(0xe5bf0004)->validate(), IllegalOpcodeError); // ldr r0, [r15, #4]!
	BOOST_CHECK_THROW(decode(0xe53f0004)->validate(), IllegalOpcodeError); // ldr r0, [r15, #-4]!
	BOOST_CHECK_THROW(decode(0xe69f0002)->validate(), IllegalOpcodeError); // ldr r0, [r15], r2
	BOOST_CHECK_THROW(decode(0xe61f0002)->validate(), IllegalOpcodeError); // ldr r0, [r15], -r2
	BOOST_CHECK_NO_THROW(decode(0xe59f0000)->validate()); // ldr r0, [r15]
	BOOST_CHECK_NO_THROW(decode(0xe59f0004)->validate()); // ldr r0, [r15, #4]
}

BOOST_AUTO_TEST_CASE(r15_offset)
{
	using namespace Emuballs;
	BOOST_CHECK_NO_THROW(decode(0xe791008e)->validate()); // ldr r0, [r1, r14, lsl #1])
	BOOST_CHECK_THROW(decode(0xe791008f)->validate(), IllegalOpcodeError); // ldr r0, [r1, r15, lsl #1])
}


BOOST_AUTO_TEST_SUITE_END()
