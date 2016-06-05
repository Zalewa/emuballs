#define BOOST_TEST_MODULE machine_armflags
#include <boost/test/unit_test.hpp>
#include <sstream>
#include "src/machine/armmachine.hpp"

using namespace Machine::Arm;

static const Flags::Bit bits[] = {
	Flags::Overflow, Flags::Carry, Flags::Negative, Flags::Zero
};

std::string bitName(Flags::Bit bit)
{
	std::stringstream ss;
	ss << "Bit " << bit;
	return ss.str();
}

void testBit(Flags::Bit bit)
{
	Flags flags;
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
	Flags flags;
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
		Flags flags;
		flags.set(bit, true);
		auto dumped = flags.dump();
		BOOST_CHECK_EQUAL(dumped, 1U << bit);
	}
}

BOOST_AUTO_TEST_CASE(store)
{
	Flags flags;
	flags.store(0xf0000000U);
	for (auto bit : bits)
	{
		BOOST_CHECK_MESSAGE(flags.test(bit), bitName(bit));
	}
	flags.store(0x60000000U);
	BOOST_CHECK(!flags.test(Flags::Overflow));
	BOOST_CHECK(flags.test(Flags::Carry));
	BOOST_CHECK(!flags.test(Flags::Negative));
	BOOST_CHECK(flags.test(Flags::Zero));
}
