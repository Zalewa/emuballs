#define BOOST_TEST_MODULE armopcode_single_data_swap
#include <boost/test/unit_test.hpp>
#include "arm_program_fixture.hpp"
#include "src/machine/armopcode_impl.hpp"
#include "src/machine/errors.hpp"

static Machine::Arm::OpcodePtr decode(uint32_t code)
{
	return Machine::Arm::opcodeSingleDataSwap(code);
}

constexpr uint32_t MEM_ADDR = 0x40000;

BOOST_FIXTURE_TEST_SUITE(suite, ArmProgramFixture)

BOOST_AUTO_TEST_CASE(swp)
{
	auto op = decode(0xe1010092); // swp r0, r2, [r1]
	machine.memory().putWord(MEM_ADDR, 0xbcde1234);
	r(0, 0xc1c1c1c1);
	r(1, MEM_ADDR);
	r(2, 0xcafebeba);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0xbcde1234);
	BOOST_CHECK_EQUAL(r(1), MEM_ADDR);
	BOOST_CHECK_EQUAL(r(2), 0xcafebeba);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_ADDR), 0xcafebeba);
}

BOOST_AUTO_TEST_CASE(swpb)
{
	auto op = decode(0xe1410092); // swp r0, r2, [r1]
	machine.memory().putWord(MEM_ADDR, 0xbcde1234);
	r(0, 0xc1c1c1c1);
	r(1, MEM_ADDR);
	r(2, 0xcafebeba);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(0), 0x34);
	BOOST_CHECK_EQUAL(r(1), MEM_ADDR);
	BOOST_CHECK_EQUAL(r(2), 0xcafebeba);
	BOOST_CHECK_EQUAL(machine.memory().word(MEM_ADDR), 0xbcde12ba);
}

BOOST_AUTO_TEST_CASE(r15)
{
	using namespace Machine;
	BOOST_CHECK_THROW(decode(0xe10f0092)->validate(), IllegalOpcodeError);
	BOOST_CHECK_THROW(decode(0xe101f092)->validate(), IllegalOpcodeError);
	BOOST_CHECK_THROW(decode(0xe101009f)->validate(), IllegalOpcodeError);
	BOOST_CHECK_NO_THROW(decode(0xe1010092)->validate());
}


BOOST_AUTO_TEST_SUITE_END()
