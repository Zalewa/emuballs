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
#include "src/emuballs/memory_impl.hpp"

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

BOOST_AUTO_TEST_CASE(pageSetContentsExact)
{
	Page p(8);
	std::vector<uint8_t> emptyExpected(8, 0);
	auto emptyContents = p.contents();
	BOOST_CHECK_EQUAL_COLLECTIONS(
		emptyExpected.begin(), emptyExpected.end(),
		emptyContents.begin(), emptyContents.end());
	std::vector<uint8_t> payload = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
	p.setContents(0, payload);
	auto filledContents = p.contents();
	BOOST_CHECK_EQUAL_COLLECTIONS(
		payload.begin(), payload.end(),
		filledContents.begin(), filledContents.end());
}

BOOST_AUTO_TEST_CASE(pageSetContentsOffset)
{
	Page p(8);
	std::vector<uint8_t> payload = {0xca, 0xfe};
	p.setContents(5, payload);
	auto filledContents = p.contents();
	std::vector<uint8_t> expected(8, 0);
	expected[5] = 0xca;
	expected[6] = 0xfe;
	BOOST_CHECK_EQUAL_COLLECTIONS(
		expected.begin(), expected.end(),
		filledContents.begin(), filledContents.end());
}

BOOST_AUTO_TEST_CASE(pageSetContentsTooMuch)
{
	Page p(8);
	std::vector<uint8_t> payload = {0xca, 0xfe, 0xde, 0xad};
	BOOST_CHECK_THROW(p.setContents(7, payload), std::out_of_range);
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

BOOST_AUTO_TEST_CASE(memoryPutDword)
{
	Memory m;
	m.putDword(0x1000, 0xcafebebadeadbeef);
	BOOST_CHECK_EQUAL(m.word(0x1000), 0xdeadbeef);
	BOOST_CHECK_EQUAL(m.word(0x1004), 0xcafebeba);
}

BOOST_AUTO_TEST_CASE(memoryDword)
{
	Memory m;
	m.putWord(0x1000, 0xa1b2c3d4);
	m.putWord(0x1004, 0xffabbc11);
	BOOST_CHECK_EQUAL(m.dword(0x1000), 0xffabbc11a1b2c3d4);
}

BOOST_AUTO_TEST_CASE(memoryPutWordOnBoundary)
{
	Memory m(1024, 512);
	m.putWord(511, 0x1289afbe);
	BOOST_CHECK_EQUAL(m.word(511), 0x1289afbeUL);
	BOOST_CHECK_EQUAL(2, m.allocatedPages().size());
}

BOOST_AUTO_TEST_CASE(memoryPutChunk)
{
	const memsize OFFSET = 508;

	Memory m(1024, 512);
	BOOST_CHECK_EQUAL(0, m.allocatedPages().size());

	std::vector<uint8_t> payload = {0xca, 0xfe, 0xba, 0xbe, 0x11, 0x22, 0x33, 0x44};
	size_t putAmount = m.putChunk(OFFSET, payload);
	BOOST_CHECK_EQUAL(8, putAmount);
	BOOST_CHECK_EQUAL(2, m.allocatedPages().size());

	std::vector<uint8_t> stored = m.chunk(OFFSET, 8);
	BOOST_CHECK_EQUAL(2, m.allocatedPages().size());

	BOOST_CHECK_EQUAL(stored.size(), payload.size());
	for (size_t idx = 0; idx < payload.size(); ++idx)
	{
		BOOST_CHECK_EQUAL(stored[idx], payload[idx]);
		BOOST_CHECK_EQUAL(stored[idx], m.byte(OFFSET + idx));
	}
}

BOOST_AUTO_TEST_CASE(memoryPutMoreThanMemoryChunk)
{
	const memsize MEMSIZE = 1024;
	const memsize OFFSET = 250;

	Memory m(MEMSIZE, 128);
	std::vector<uint8_t> payload(MEMSIZE * 2, 0xcc);
	payload[MEMSIZE - OFFSET - 1] = 0x40;
	memsize putAmount = m.putChunk(OFFSET, payload);

	BOOST_CHECK_EQUAL(MEMSIZE - OFFSET, putAmount);
	BOOST_CHECK_EQUAL(7, m.allocatedPages().size());
	BOOST_CHECK_EQUAL(0x40, m.byte(MEMSIZE - 1));
}

/** Extraction of chunk should not allocate pages */
BOOST_AUTO_TEST_CASE(memoryChunkSize)
{
	const memsize MEMSIZE = 1024;
	const memsize PAGESIZE = 128;

	Memory m(MEMSIZE, PAGESIZE);
	BOOST_CHECK_EQUAL(0, m.allocatedPages().size());

	std::vector<uint8_t> chunk = m.chunk(0, MEMSIZE);
	BOOST_CHECK_EQUAL(0, m.allocatedPages().size());
	BOOST_CHECK_EQUAL(MEMSIZE, chunk.size());

	std::vector<uint8_t> chunkLarger = m.chunk(512, MEMSIZE * 2);
	BOOST_CHECK_EQUAL(0, m.allocatedPages().size());
	BOOST_CHECK_EQUAL(MEMSIZE - 512, chunkLarger.size());
}

BOOST_AUTO_TEST_CASE(memoryChunk)
{
	Memory m(1024, 128);
	m.putByte(0, 0x40);
	std::vector<uint8_t> chunk = m.chunk(0, 1);
	BOOST_CHECK_EQUAL(1, chunk.size());
	BOOST_CHECK_EQUAL(chunk[0], 0x40);

	m.putByte(128, 0x71);
	std::vector<uint8_t> chunk2 = m.chunk(128, 1);
	BOOST_CHECK_EQUAL(1, chunk2.size());
	BOOST_CHECK_EQUAL(chunk2[0], 0x71);

	m.putByte(1000, 0xfe);
	m.putByte(1001, 0xed);
	std::vector<uint8_t> chunk3 = m.chunk(1000, 2);
	BOOST_CHECK_EQUAL(2, chunk3.size());
	BOOST_CHECK_EQUAL(chunk3[0], 0xfe);
	BOOST_CHECK_EQUAL(chunk3[1], 0xed);

	m.putByte(1022, 0xa1);
	m.putByte(1023, 0xb2);
	std::vector<uint8_t> chunkReadMore = m.chunk(1022, 10512);
	BOOST_CHECK_EQUAL(2, chunkReadMore.size());
	BOOST_CHECK_EQUAL(chunkReadMore[0], 0xa1);
	BOOST_CHECK_EQUAL(chunkReadMore[1], 0xb2);
}

BOOST_AUTO_TEST_CASE(memoryChunkToPointer)
{
	Memory m(1024, 128);
	m.putByte(0, 0x40);
	std::vector<uint8_t> chunk;
	chunk.reserve(1);
	auto read = m.chunk(0, 1, chunk.data());
	BOOST_CHECK_EQUAL(1, read);
	BOOST_CHECK_EQUAL(chunk[0], 0x40);

	m.putByte(128, 0x71);
	std::vector<uint8_t> chunk2;
	chunk2.reserve(1);
	auto read2 = m.chunk(128, 1, chunk2.data());
	BOOST_CHECK_EQUAL(1, read2);
	BOOST_CHECK_EQUAL(chunk2[0], 0x71);

	m.putByte(1000, 0xfe);
	m.putByte(1001, 0xed);
	std::vector<uint8_t> chunk3;
	chunk3.reserve(2);
	auto read3 = m.chunk(1000, 2, chunk3.data());
	BOOST_CHECK_EQUAL(2, read3);
	BOOST_CHECK_EQUAL(chunk3[0], 0xfe);
	BOOST_CHECK_EQUAL(chunk3[1], 0xed);

	m.putByte(1022, 0xa1);
	m.putByte(1023, 0xb2);
	std::vector<uint8_t> chunkReadMore;
	chunkReadMore.reserve(300); // 300 is different than 2 and 10512.
	auto readMore = m.chunk(1022, 10512, chunkReadMore.data());
	BOOST_CHECK_EQUAL(2, readMore);
	BOOST_CHECK_EQUAL(chunkReadMore[0], 0xa1);
	BOOST_CHECK_EQUAL(chunkReadMore[1], 0xb2);
}

BOOST_AUTO_TEST_CASE(memoryMaxSize)
{
	Memory m(512);
	BOOST_CHECK_NO_THROW(m.putByte(511, 1));
	BOOST_CHECK_THROW(m.putByte(512, 1), std::out_of_range);
}
