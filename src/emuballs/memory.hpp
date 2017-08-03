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
	Page(memsize size = 0)
	{
		bytes.resize(size);
	}

	const uint8_t &operator[](memsize index) const
	{
		if (index >= bytes.size())
			throw std::out_of_range("index outsize page size");
		return bytes[index];
	}

	uint8_t &operator[](memsize index)
	{
		return const_cast<uint8_t&>( static_cast<const Page&>(*this)[index] );
	}

	const std::vector<uint8_t> &contents() const
	{
		return bytes;
	}

	std::vector<uint8_t> &contents()
	{
		return bytes;
	}

	void setContents(memsize offset, const std::vector<uint8_t> &bytes)
	{
		setContents(offset, bytes.begin(), bytes.end());
	}

	template<class iter>
	void setContents(memsize offset, iter bytesBegin, iter bytesEnd)
	{
		size_t bytesSize = bytesEnd - bytesBegin;
		if (offset + bytesSize > size())
			throw std::out_of_range("contents must be within page size");
		std::copy(bytesBegin, bytesEnd, this->bytes.begin() + offset);
	}

	memsize size() const
	{
		return bytes.size();
	}

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
	TrackedMemory(Memory &memory)
		: memory(memory)
	{
	}

	memsize putChunk(memsize address, const std::vector<uint8_t> &chunk)
	{
		auto ret = memory.putChunk(address, chunk);
		memory.execObservers(address, ret, Access::Write);
		return ret;
	}

	memsize putChunk(memsize address, const uint8_t *begin, memsize length)
	{
		auto ret = memory.putChunk(address, begin, length);
		memory.execObservers(address, ret, Access::Write);
		return ret;
	}

	std::vector<uint8_t> chunk(memsize address, memsize length) const
	{
		memory.execObservers(address, length, Access::PreRead);
		auto ret = memory.chunk(address, length);
		memory.execObservers(address, ret.size(), Access::Read);
		return ret;
	}

	memsize chunk(memsize address, memsize length, uint8_t *begin) const
	{
		memory.execObservers(address, length, Access::PreRead);
		auto ret = memory.chunk(address, length, begin);
		memory.execObservers(address, ret, Access::Read);
		return ret;
	}

	void putByte(memsize address, uint8_t value)
	{
		memory.putByte(address, value);
		memory.execObservers(address, 0, Access::Write);
	}

	uint8_t byte(memsize address) const
	{
		memory.execObservers(address, 0, Access::PreRead);
		auto ret = memory.byte(address);
		memory.execObservers(address, 0, Access::Read);
		return ret;
	}

	void putWord(memsize address, uint32_t value)
	{
		memory.putWord(address, value);
		memory.execObservers(address, 0, Access::Write);
	}

	uint32_t word(memsize address) const
	{
		memory.execObservers(address, 0, Access::PreRead);
		auto ret = memory.word(address);
		memory.execObservers(address, 0, Access::Read);
		return ret;
	}

	void putDword(memsize address, uint64_t value)
	{
		memory.putDword(address, value);
		memory.execObservers(address, 0, Access::Write);
	}

	uint64_t dword(memsize address) const
	{
		memory.execObservers(address, 0, Access::PreRead);
		auto ret = memory.dword(address);
		memory.execObservers(address, 0, Access::Read);
		return ret;
	}

private:
	Memory &memory;
};

}
