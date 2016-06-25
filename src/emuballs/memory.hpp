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

namespace Emuballs
{
class Page
{
public:
	Page(size_t size = 0);

	const uint8_t &operator[](size_t index) const;
	uint8_t &operator[](size_t index)
	{
		return const_cast<uint8_t&>( static_cast<const Page&>(*this)[index] );
	}

	size_t size() const;

private:
	std::vector<uint8_t> bytes;
};

class Memory
{
public:
	Memory(size_t totalSize = SIZE_MAX, size_t pageSize = 4096);

	void putByte(uint32_t address, uint8_t value);
	uint8_t byte(uint32_t address) const;

	void putWord(uint32_t address, uint32_t value);
	uint32_t word(uint32_t address) const;

private:
	const Page &page(uint32_t address) const;
	Page &page(uint32_t address)
	{
		return const_cast<Page&>( static_cast<const Memory&>(*this).page(address) );
	}

	uint32_t pageAddress(uint32_t memAddress) const;
	uint32_t pageOffset(uint32_t memAddress) const;

	size_t size;
	size_t pageSize;
	mutable std::map<uint32_t, Page> pages;
};
}
