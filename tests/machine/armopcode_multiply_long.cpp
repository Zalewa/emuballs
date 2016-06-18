#define BOOST_TEST_MODULE armopcode_multiply_long
#include <boost/test/unit_test.hpp>
#include "arm_program_fixture.hpp"
#include "src/machine/armopcode_impl.hpp"
#include "src/machine/errors.hpp"

static Machine::Arm::OpcodePtr decode(uint32_t code)
{
	auto op = Machine::Arm::opcodeMultiplyLong(code);
	op->validate();
	return op;
}

BOOST_FIXTURE_TEST_SUITE(suite, ArmProgramFixture)

BOOST_AUTO_TEST_CASE(mull_unsigned_cpsr)
{
	auto op = decode(0xe0932796); // umulls r2, r3, r6, r7

	flags().zero(false);
	flags().negative(false);
	r(2, 0);
	r(3, 0);
	r(6, 0xffffffff);
	r(7, 0xffffffff);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(2), 0x00000001);
	BOOST_CHECK_EQUAL(r(3), 0xfffffffe);
	BOOST_CHECK(flags().negative());
	BOOST_CHECK(!flags().zero());

	flags().zero(false);
	flags().negative(false);
	r(2, 0);
	r(3, 0);
	r(6, 0x7fffffff);
	r(7, 0xffffffff);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(2), 0x80000001);
	BOOST_CHECK_EQUAL(r(3), 0x7ffffffe);
	BOOST_CHECK(!flags().negative());
	BOOST_CHECK(!flags().zero());

	flags().zero(false);
	flags().negative(false);
	r(2, 0);
	r(3, 0);
	r(6, 0x0);
	r(7, 0xffffffff);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(2), 0);
	BOOST_CHECK_EQUAL(r(3), 0);
	BOOST_CHECK(!flags().negative());
	BOOST_CHECK(flags().zero());
}

BOOST_AUTO_TEST_CASE(mull_unsigned)
{
	auto op = decode(0xe0832796); // umull r2, r3, r6, r7

	flags().zero(false);
	flags().negative(false);
	r(2, 0);
	r(3, 0);
	r(6, 0xffffffff);
	r(7, 0xffffffff);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(2), 0x00000001);
	BOOST_CHECK_EQUAL(r(3), 0xfffffffe);
	BOOST_CHECK(!flags().negative());
	BOOST_CHECK(!flags().zero());

	flags().zero(true);
	flags().negative(true);
	r(2, 0);
	r(3, 0);
	r(6, 0xefffffff);
	r(7, 0xffffffff);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(2), 0x10000001);
	BOOST_CHECK_EQUAL(r(3), 0xeffffffe);
	BOOST_CHECK(flags().negative());
	BOOST_CHECK(flags().zero());

	flags().zero(false);
	flags().negative(false);
	r(2, 0);
	r(3, 0);
	r(6, 0x0);
	r(7, 0xffffffff);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(2), 0);
	BOOST_CHECK_EQUAL(r(3), 0);
	BOOST_CHECK(!flags().negative());
	BOOST_CHECK(!flags().zero());
}

BOOST_AUTO_TEST_CASE(mull_unsigned_accumulate_cpsr)
{
	auto op = decode(0xe0b32796); // umlals r2, r3, r6, r7

	flags().zero(false);
	flags().negative(false);
	r(2, 10);
	r(3, 14);
	r(6, 0xffffffff);
	r(7, 0xffffffff);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(2), 0x00000001 + 10);
	BOOST_CHECK_EQUAL(r(3), 0xfffffffe + 14);
	BOOST_CHECK(!flags().negative());
	BOOST_CHECK(!flags().zero());

	flags().zero(false);
	flags().negative(false);
	r(2, 0xffffffff);
	r(3, 1);
	r(6, 0xffffffff);
	r(7, 0xffffffff);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(2), 0);
	BOOST_CHECK_EQUAL(r(3), 0);
	BOOST_CHECK(!flags().negative());
	BOOST_CHECK(flags().zero());

	flags().zero(false);
	flags().negative(false);
	r(2, 14);
	r(3, 10);
	r(6, 0xefffffff);
	r(7, 0xffffffff);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(2), 0x10000001 + 14);
	BOOST_CHECK_EQUAL(r(3), 0xeffffffe + 10);
	BOOST_CHECK(flags().negative());
	BOOST_CHECK(!flags().zero());

	flags().zero(false);
	flags().negative(false);
	r(2, 0);
	r(3, 0);
	r(6, 0x0);
	r(7, 0xffffffff);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(2), 0);
	BOOST_CHECK_EQUAL(r(3), 0);
	BOOST_CHECK(!flags().negative());
	BOOST_CHECK(flags().zero());

	flags().zero(false);
	flags().negative(false);
	r(2, 1);
	r(3, 0);
	r(6, 0x0);
	r(7, 0xffffffff);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(2), 1);
	BOOST_CHECK_EQUAL(r(3), 0);
	BOOST_CHECK(!flags().negative());
	BOOST_CHECK(!flags().zero());
}

BOOST_AUTO_TEST_CASE(mull_unsigned_accumulate)
{
	auto op = decode(0xe0a32796); // umlal r2, r3, r6, r7

	flags().zero(false);
	flags().negative(false);
	r(2, 10);
	r(3, 14);
	r(6, 0xffffffff);
	r(7, 0xffffffff);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(2), 0x00000001 + 10);
	BOOST_CHECK_EQUAL(r(3), 0xfffffffe + 14);
	BOOST_CHECK(!flags().negative());
	BOOST_CHECK(!flags().zero());

	flags().zero(true);
	flags().negative(true);
	r(2, 14);
	r(3, 10);
	r(6, 0xefffffff);
	r(7, 0xffffffff);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(2), 0x10000001 + 14);
	BOOST_CHECK_EQUAL(r(3), 0xeffffffe + 10);
	BOOST_CHECK(flags().negative());
	BOOST_CHECK(flags().zero());

	flags().zero(false);
	flags().negative(true);
	r(2, 0);
	r(3, 0);
	r(6, 0x0);
	r(7, 0xffffffff);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(2), 0);
	BOOST_CHECK_EQUAL(r(3), 0);
	BOOST_CHECK(flags().negative());
	BOOST_CHECK(!flags().zero());

	flags().zero(false);
	flags().negative(false);
	r(2, 1);
	r(3, 0);
	r(6, 0x0);
	r(7, 0xffffffff);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(2), 1);
	BOOST_CHECK_EQUAL(r(3), 0);
	BOOST_CHECK(!flags().negative());
	BOOST_CHECK(!flags().zero());
}

BOOST_AUTO_TEST_CASE(mull_signed_cpsr)
{
	auto op = decode(0xe0d32796); // smulls r2, r3, r6, r7

	flags().zero(false);
	flags().negative(false);
	r(2, 0);
	r(3, 0);
	r(6, 0xffffffff);
	r(7, 0xffffffff);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(2), 1);
	BOOST_CHECK_EQUAL(r(3), 0);
	BOOST_CHECK(!flags().negative());
	BOOST_CHECK(!flags().zero());

	flags().zero(false);
	flags().negative(false);
	r(2, 0);
	r(3, 0);
	r(6, 0x7fffffff);
	r(7, 0xffffffff);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(2), 0x80000001);
	BOOST_CHECK_EQUAL(r(3), 0xffffffff);
	BOOST_CHECK(flags().negative());
	BOOST_CHECK(!flags().zero());

	flags().zero(false);
	flags().negative(false);
	r(2, 0);
	r(3, 0);
	r(6, 0x0);
	r(7, 0xffffffff);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(2), 0);
	BOOST_CHECK_EQUAL(r(3), 0);
	BOOST_CHECK(!flags().negative());
	BOOST_CHECK(flags().zero());
}

BOOST_AUTO_TEST_CASE(mull_signed_accumulate_cpsr)
{
	auto op = decode(0xe0f32796); // smlals r2, r3, r6, r7

	flags().zero(false);
	flags().negative(false);
	r(2, 10);
	r(3, 0);
	r(6, 0xffffffff);
	r(7, 0xffffffff);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(2), 11);
	BOOST_CHECK_EQUAL(r(3), 0);
	BOOST_CHECK(!flags().negative());
	BOOST_CHECK(!flags().zero());

	flags().zero(false);
	flags().negative(false);
	r(2, 4);
	r(3, 6);
	r(6, 0xefffffff);
	r(7, 0xffffffff);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(2), 0x10000001 + 4);
	BOOST_CHECK_EQUAL(r(3), 6);
	BOOST_CHECK(!flags().negative());
	BOOST_CHECK(!flags().zero());

	flags().zero(false);
	flags().negative(false);
	r(2, 3);
	r(3, 6);
	r(6, 0x0);
	r(7, 0xffffffff);
	op->execute(machine);
	BOOST_CHECK_EQUAL(r(2), 3);
	BOOST_CHECK_EQUAL(r(3), 6);
	BOOST_CHECK(!flags().negative());
	BOOST_CHECK(!flags().zero());
}

BOOST_AUTO_TEST_CASE(r15)
{
	using namespace Machine;
	BOOST_CHECK_THROW(decode(0xe0810f94)->validate(), IllegalOpcodeError); // umull r0, r1, r4, r15
	BOOST_CHECK_THROW(decode(0xe081059f)->validate(), IllegalOpcodeError); // umull r0, r1, r15, r5
	BOOST_CHECK_THROW(decode(0xe08f0594)->validate(), IllegalOpcodeError); // umull r0, r15, r4, r5
	BOOST_CHECK_THROW(decode(0xe081f594)->validate(), IllegalOpcodeError); // umull r15, r1, r4, r5
	BOOST_CHECK_NO_THROW(decode(0xe0810594)->validate()); // umull r0, r1, r4, r5
}

BOOST_AUTO_TEST_CASE(same_regs)
{
	using namespace Machine;
	BOOST_CHECK_THROW(decode(0xe0811594)->validate(), IllegalOpcodeError); // umull r1, r1, r4, r5
	BOOST_CHECK_THROW(decode(0xe0810591)->validate(), IllegalOpcodeError); // umull r0, r1, r1, r5
	BOOST_CHECK_THROW(decode(0xe0814594)->validate(), IllegalOpcodeError); // umull r4, r1, r4, r5
	BOOST_CHECK_THROW(decode(0xe0844594)->validate(), IllegalOpcodeError); // umull r4, r4, r4, r5
	BOOST_CHECK_NO_THROW(decode(0xe0810594)->validate()); // umull r0, r1, r4, r5
	BOOST_CHECK_NO_THROW(decode(0xe0810494)->validate()); // umull r0, r1, r4, r4
	BOOST_CHECK_NO_THROW(decode(0xe0810194)->validate()); // umull r0, r1, r5, r1
	BOOST_CHECK_NO_THROW(decode(0xe0810094)->validate()); // umull r0, r1, r5, r0
}

BOOST_AUTO_TEST_SUITE_END()
