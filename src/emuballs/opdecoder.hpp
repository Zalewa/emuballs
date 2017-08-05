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
#include <istream>
#include <map>
#include <vector>
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
	OpDecoder();
	OpDecoder(const OpDecoder &other);
	OpDecoder(OpDecoder && other) noexcept;
	OpDecoder &operator=(OpDecoder other);
	friend void swap(OpDecoder &a, OpDecoder &b) noexcept;

	OpcodePtr next(std::istream &);
	OpcodePtr decode(memsize address, uint32_t instruction);

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
		uint32_t instruction = 0;
		OpcodePtr opcode = nullptr;
	};

	std::map<memsize, std::vector<Op>> opPages;
	memsize currentPageAddress = 0;
	std::vector<Op> *currentPage = nullptr;

	std::vector<Op> *createPage(memsize address)
	{
		opPages.emplace(address, std::vector<Op>(OP_PAGE_SIZE));
		return &opPages.find(0)->second;
	}

	OpcodePtr findOpcodeOnCurrentPage(memsize address, uint32_t instruction)
	{
		Op &op = (*currentPage)[address & OP_PAGE_OFFSET_MASK];
		if (op.instruction == instruction)
			return op.opcode;
		return nullptr;
	}

	void saveOpcodeOnCurrentPage(memsize address, uint32_t instruction, OpcodePtr opcode)
	{
		(*currentPage)[address & OP_PAGE_OFFSET_MASK] = Op {
			.instruction = instruction,
			.opcode = opcode
		};
	}

};

}

}
