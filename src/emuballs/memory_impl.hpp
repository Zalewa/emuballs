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
#pragma once

#include <cstdint>
#include <vector>
#include "emuballs/memory.hpp"

namespace Emuballs
{
class Page
{
public:
	Page(memsize size = 0);

	const uint8_t &operator[](memsize index) const;
	uint8_t &operator[](memsize index)
	{
		return const_cast<uint8_t&>( static_cast<const Page&>(*this)[index] );
	}

	const std::vector<uint8_t> &contents() const;
	std::vector<uint8_t> &contents();
	void setContents(memsize offset, const std::vector<uint8_t> &bytes);

	template<class iter>
	void setContents(memsize offset, iter bytesBegin, iter bytesEnd)
	{
		size_t bytesSize = bytesEnd - bytesBegin;
		if (offset + bytesSize > size())
			throw std::out_of_range("contents must be within page size");
		std::copy(bytesBegin, bytesEnd, this->bytes.begin() + offset);
	}

	memsize size() const;

private:
	std::vector<uint8_t> bytes;
};

class MemoryStreamReader
{
public:
	MemoryStreamReader(const Memory &memory, memsize startOffset = 0);

	uint32_t readUint32();
	uint64_t readUint64();
	void skip(memsize amount);

private:
	const Memory &memory;
	memsize _offset;
};

class MemoryStreamWriter
{
public:
	MemoryStreamWriter(Memory &memory, memsize startOffset = 0);

	void writeUint32(uint32_t value);
	void writeUint64(uint64_t value);
	void skip(memsize amount);

private:
	Memory &memory;
	memsize _offset;
};

class TrackedMemory
{
public:
	TrackedMemory(Memory &memory);

	memsize putChunk(memsize address, const std::vector<uint8_t> &chunk);
	std::vector<uint8_t> chunk(memsize address, memsize length) const;

	void putByte(memsize address, uint8_t value);
	uint8_t byte(memsize address) const;

	void putWord(memsize address, uint32_t value);
	uint32_t word(memsize address) const;

	void putDword(memsize address, uint64_t value);
	uint64_t dword(memsize address) const;

	Memory &memory;
};

}
