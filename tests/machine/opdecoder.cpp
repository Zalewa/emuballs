#define BOOST_TEST_MODULE machine_opdecoder
#include <boost/test/unit_test.hpp>
#include <sstream>
#include "src/machine/opdecoder.hpp"

using namespace Machine::Arm;

struct Fixture
{
	OpDecoder decoder;
	std::stringstream io;

	void prepare(uint32_t code)
	{
		io.write(reinterpret_cast<char*>(&code), sizeof(uint32_t));
		io.seekg(0);
	}
};

BOOST_FIXTURE_TEST_SUITE(opdecoder, Fixture);

BOOST_AUTO_TEST_CASE(invalidConditional)
{
	prepare(0xf0000000);
	BOOST_CHECK_THROW(decoder.next(io), OpDecodeError);
}

BOOST_AUTO_TEST_CASE(empty)
{
	BOOST_CHECK_THROW(decoder.next(io), std::ios::failure);
}

BOOST_AUTO_TEST_CASE(tooShort)
{
	io.write("\xba\x51\x32", 3);
	io.seekg(0);
	BOOST_CHECK_THROW(decoder.next(io), std::ios::failure);
}

BOOST_AUTO_TEST_CASE(misaligned)
{
	io.write("\x00\x00\x00\xe0\x00\x00\x00", 7);
	io.seekg(4);
	BOOST_CHECK_THROW(decoder.next(io), std::ios::failure);
}

BOOST_AUTO_TEST_SUITE_END();
