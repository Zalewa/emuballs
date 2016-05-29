#define BOOST_TEST_MODULE machine_armflags
#include <boost/test/unit_test.hpp>
#include <boost/test/parameterized_test.hpp>
#include <sstream>
#include "src/machine/armmachine.hpp"

using namespace Machine;

static const ArmFlags::Bit bits[] = {
	ArmFlags::Overflow, ArmFlags::Carry, ArmFlags::Negative, ArmFlags::Zero
};

std::string bitName(ArmFlags::Bit bit)
{
	std::stringstream ss;
	ss << "Bit " << bit;
	return ss.str();
}

void testBit(ArmFlags::Bit bit)
{
	ArmFlags flags;
	BOOST_CHECK_MESSAGE(!flags.test(bit), bitName(bit));
	flags.set(bit, true);
	BOOST_CHECK_MESSAGE(flags.test(bit), bitName(bit));
	flags.set(bit, false);
	BOOST_CHECK_MESSAGE(!flags.test(bit), bitName(bit));
}

BOOST_AUTO_TEST_CASE(checkBits)
{
	for (auto bit : bits)
	{
		testBit(bit);
	}
}

BOOST_AUTO_TEST_CASE(dumpAll)
{
	ArmFlags flags;
	for (auto bit : bits)
	{
		flags.set(bit, true);
	}
	auto dumped = flags.dump();
	BOOST_CHECK_EQUAL(dumped, 0xf0000000U);
}

BOOST_AUTO_TEST_CASE(dumpEach)
{
	for (auto bit : bits)
	{
		ArmFlags flags;
		flags.set(bit, true);
		auto dumped = flags.dump();
		BOOST_CHECK_EQUAL(dumped, 1U << bit);
	}
}

BOOST_AUTO_TEST_CASE(store)
{
	ArmFlags flags;
	flags.store(0xf0000000U);
	for (auto bit : bits)
	{
		BOOST_CHECK_MESSAGE(flags.test(bit), bitName(bit));
	}
	flags.store(0x60000000U);
	BOOST_CHECK(!flags.test(ArmFlags::Overflow));
	BOOST_CHECK(flags.test(ArmFlags::Carry));
	BOOST_CHECK(!flags.test(ArmFlags::Negative));
	BOOST_CHECK(flags.test(ArmFlags::Zero));
}
