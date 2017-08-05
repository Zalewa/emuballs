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

#include <array>
#include <cstdint>
#include <istream>
#include <map>
#include "armopcode.hpp"
#include "memory.hpp"

namespace Emuballs
{

namespace Arm
{

class Opcode;


class OpDecoder
{
public:
	OpDecoder()
	{
	}

	OpDecoder(const OpDecoder &other)
	{
		// All members of this class are cache.
		//
		// They need not be copied, because they can be
		// regenerated under normal operation.
		//
		// Copying them might actually be more expensive than
		// allowing them to be regenerated from scratch.
	}

	OpDecoder(OpDecoder && other) noexcept
	{
		swap(*this, other);
	}

	OpDecoder &operator=(OpDecoder other)
	{
		swap(*this, other);
		return *this;
	}

	friend void swap(OpDecoder &a, OpDecoder &b) noexcept;

	Opcode* next(std::istream &);
	Opcode* decode(memsize address, uint32_t instruction);

private:
	static const int OP_ARRAY_SHIFT = 20;
	static const int OP_ARRAY_MASK = 0xfff;
	static const int NUM_OPS_ARRAYS = OP_ARRAY_MASK + 1;
	std::map<uint32_t, OpcodePtr> decodedOps[NUM_OPS_ARRAYS];

	static const memsize OP_PAGE_OFFSET_MASK = 0xffff;
	static const memsize OP_PAGE_MASK = ~OP_PAGE_OFFSET_MASK;
	static const memsize OP_PAGE_SIZE = (OP_PAGE_OFFSET_MASK) + 1;
	struct Op
	{
		Opcode* opcode = nullptr;
	};

	typedef std::array<Op, OP_PAGE_SIZE> OpPage;

	std::map<memsize, OpPage> opPages;
	memsize currentPageAddress = 0;
	OpPage *currentPage = nullptr;

	OpPage *createPage(memsize address)
	{
		opPages.emplace(address, OpPage());
		return &opPages.find(0)->second;
	}

	Opcode *findDecodedOp(uint32_t instruction)
	{
		auto &decodedMap = decodedOps[decodedMapAddress(instruction)];
		auto cachedOpIt = decodedMap.find(instruction);
		if (cachedOpIt != decodedMap.end())
		{
			return cachedOpIt->second.get();
		}
		return nullptr;
	}

	uint32_t decodedMapAddress(uint32_t instruction) noexcept
	{
		return (instruction >> OP_ARRAY_SHIFT) & OP_ARRAY_MASK;
	}

	Opcode *findOpcodeOnCurrentPage(memsize address, uint32_t instruction)
	{
		Opcode *op = (*currentPage)[address & OP_PAGE_OFFSET_MASK].opcode;
		if (op != nullptr && op->code() == instruction)
			return op;
		return nullptr;
	}

	void saveOpcodeOnCurrentPage(memsize address, Opcode* opcode)
	{
		(*currentPage)[address & OP_PAGE_OFFSET_MASK] = Op {
			.opcode = opcode
		};
	}
};

}

}
