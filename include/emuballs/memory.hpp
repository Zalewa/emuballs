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
#include <functional>
#include <vector>
#include "emuballs/access.hpp"
#include "emuballs/bitmask_enum.hpp"
#include "emuballs/dptr.hpp"
#include "emuballs/export.h"

namespace Emuballs
{
typedef size_t memsize;
typedef std::function<void(memsize, Access)> memobserver;
typedef uint32_t memobserver_id;

class EMUBALLS_API Memory
{
public:
	static const memobserver_id NO_OBSERVER = 0;

	Memory(memsize totalSize = SIZE_MAX, memsize pageSize = 4096);
	virtual ~Memory();

	std::vector<memsize> allocatedPages() const;

	/**
	 * @return Amount of data actually put; can't exceed `chunk.size()`.
	 */
	memsize putChunk(memsize address, const std::vector<uint8_t> &chunk);
	/**
	 * @return Amount of data actually put; can't exceed `length`.
	 */
	memsize putChunk(memsize address, const uint8_t *begin, memsize length);

	std::vector<uint8_t> chunk(memsize address, memsize length) const;
	memsize chunk(memsize address, memsize length, uint8_t *begin) const;

	void putByte(memsize address, uint8_t value);
	uint8_t byte(memsize address) const;

	void putWord(memsize address, uint32_t value);
	uint32_t word(memsize address) const;

	void putDword(memsize address, uint64_t value);
	uint64_t dword(memsize address) const;

	/**
	 * Plain C pointer to memory chunk under this address.
	 *
	 * This is highly unsafe as the memory is split into pages.
	 *
	 * `pageSize() - (address % pageSize())` determines the length
	 * of the buffer pointed at this pointer. Memory is only continuous
	 * within a Page. Pages break continuity.
	 */
	uint8_t *ptr(memsize address);
	memsize pageSize() const;
	memsize size() const;

	// TODO: this pattern will probably fit in a separate class.
	memobserver_id observe(memsize address, memsize length, memobserver observer, Access events);
	void unobserve(memobserver_id id);

private:
	friend class TrackedMemory;

	DPtr<Memory> d;

	void execObservers(memsize address, memsize length, Access events);
};

}
