#define BOOST_TEST_MODULE machine_programs
#include <boost/test/unit_test.hpp>
#include <iterator>
#include "src/machine/armmachine.hpp"
#include "src/machine/memory.hpp"

using namespace Machine::Arm;

struct Fixture
{
	static constexpr auto STEP_LIMIT = 10000;

	Machine::Arm::Machine machine;
	Machine::Arm::Machine snapshot;

	regval r(int which)
	{
		return machine.cpu().regs()[which];
	}

	void r(int which, regval val)
	{
		machine.cpu().regs()[which] = val;
	}

	void load(const uint32_t *begin, const uint32_t *end)
	{
		machine = Machine::Arm::Machine();
		auto addr = 0;
		for (auto addr = 0; begin != end; ++begin, addr += sizeof(*begin))
		{
			machine.memory().putWord(addr, *begin);
		}
		machine.cpu().regs().pc() = 0;
		machine.cpu().regs().lr() = addr + PREFETCH_SIZE;
		snapshot = machine;
	}

	void reset()
	{
		machine = snapshot;
	}

	void runProgram()
	{
		auto endAddress = machine.cpu().regs().lr();
		auto limit = STEP_LIMIT;
		for (; limit > 0 && machine.cpu().regs().pc() < endAddress; --limit)
		{
			machine.cycle();
		}
		BOOST_WARN_EQUAL(machine.cpu().regs().pc(), endAddress);
		BOOST_REQUIRE_NE(0, limit);
	}
};

BOOST_FIXTURE_TEST_SUITE(suite, Fixture);

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

	constexpr auto AFFECTED_REGS = 12;
	constexpr auto PARAM = 0xa1b2c3d4;
	load(std::begin(code), std::end(code));
	for (auto i = 0; i < AFFECTED_REGS; ++i)
	{
		r(i, 0);
	}
	r(0, PARAM);
	runProgram();
	for (auto i = 0; i < AFFECTED_REGS; ++i)
	{
		BOOST_CHECK_EQUAL(r(i), PARAM);
	}
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


BOOST_AUTO_TEST_SUITE_END()
