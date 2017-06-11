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
#include <map>
#include <vector>
#include "dptr.hpp"
#include "export.h"

namespace Emuballs
{
typedef size_t memsize;

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
	void setContents(memsize offset, const std::vector<uint8_t> &bytes);

	memsize size() const;

private:
	std::vector<uint8_t> bytes;
};

class EMUBALLS_API Memory
{
public:
	Memory(memsize totalSize = SIZE_MAX, memsize pageSize = 4096);

	std::vector<memsize> allocatedPages() const;

	/**
	 * @return Amount of data actually put; can't exceed `size() - address`.
	 */
	memsize putChunk(memsize address, const std::vector<uint8_t> &chunk);
	std::vector<uint8_t> chunk(memsize address, memsize length) const;

	void putByte(memsize address, uint8_t value);
	uint8_t byte(memsize address) const;

	void putWord(memsize address, uint32_t value);
	uint32_t word(memsize address) const;

	memsize pageSize() const;
	memsize size() const;

private:
	DPtr<Memory> d;
};
}
