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
#define BOOST_TEST_MODULE machine_opdecoder
#include <boost/test/unit_test.hpp>
#include <sstream>
#include "src/emuballs/errors.hpp"
#include "src/emuballs/opdecoder.hpp"

using namespace Emuballs::Arm;

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

BOOST_AUTO_TEST_CASE(badOpcode)
{
	BOOST_CHECK_THROW(decoder.decode(0xf6000010), Emuballs::OpDecodeError);
	io.write("\x10\x00\x00\xf6", 4);
	io.seekg(0);
	BOOST_CHECK_THROW(decoder.next(io), Emuballs::OpDecodeError);
}

BOOST_AUTO_TEST_CASE(goodOpcodeButBadCombination)
{
	BOOST_CHECK_THROW(decoder.decode(0xe791008f), Emuballs::IllegalOpcodeError);
	io.write("\x8f\x00\x91\xe7", 4);
	io.seekg(0);
	BOOST_CHECK_THROW(decoder.next(io), Emuballs::IllegalOpcodeError);
}

BOOST_AUTO_TEST_SUITE_END();
