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
#define BOOST_TEST_MODULE machine_memory
#include <boost/test/unit_test.hpp>
#include "src/emuballs/memory.hpp"

using namespace Emuballs;

BOOST_AUTO_TEST_CASE(pageIndexOperator)
{
	Page p(512);
	p[0] = 0xff;
	BOOST_CHECK_EQUAL(p[0], 0xff);
}

BOOST_AUTO_TEST_CASE(pageIndexOperatorConst)
{
	Page p(512);
	p[0] = 0xff;
	const Page &pc = p;
	BOOST_CHECK_EQUAL(pc[0], 0xff);
}

BOOST_AUTO_TEST_CASE(pageFill)
{
	Page p(512);
	for (size_t idx = 0; idx < p.size(); ++idx)
	{
		p[idx] = idx % 256;
	}
	for (size_t idx = 0; idx < p.size(); ++idx)
	{
		BOOST_CHECK_EQUAL(p[idx], idx % 256);
	}
}

BOOST_AUTO_TEST_CASE(pageOutOfRangeWrite)
{
	Page p(512);
	BOOST_CHECK_THROW(p[p.size()] = 0, std::out_of_range);
}

BOOST_AUTO_TEST_CASE(pageOutOfRangeRead)
{
	Page p(512);
	BOOST_CHECK_THROW(p[p.size()], std::out_of_range);
}

BOOST_AUTO_TEST_CASE(memoryPutByte)
{
	Memory m;
	m.putByte(0xdeadbeef, 255);
	BOOST_CHECK_EQUAL(m.byte(0xdeadbeef), 255);
}

BOOST_AUTO_TEST_CASE(memoryPutWord)
{
	Memory m;
	m.putWord(0x1000, 0xdeadbeef);
	BOOST_CHECK_EQUAL(m.word(0x1000), 0xdeadbeef);
}

BOOST_AUTO_TEST_CASE(memoryPutWordOnBoundary)
{
	Memory m(1024, 512);
	m.putWord(511, 0x1289afbe);
	BOOST_CHECK_EQUAL(m.word(511), 0x1289afbeUL);
}

BOOST_AUTO_TEST_CASE(memoryMaxSize)
{
	Memory m(512);
	BOOST_CHECK_NO_THROW(m.putByte(511, 1));
	BOOST_CHECK_THROW(m.putByte(512, 1), std::out_of_range);
}
